#include "parcours2.h"

#include "builtins/boolean.h"
#include "builtins/echo.h"
#include "include.h"
#include "lexer.h"
#include "parcours.h"
#include "parcours3.h"
#include "parser.h"

extern struct global_var global_var;
int execute_while(struct ast *arbre)
{
    if (arbre->nb_child != 2)
    {
        fprintf(stderr, "Missing arguments");
        return 2;
    }
    struct ast *tempo = arbre->child[0];

    struct ast *execution = arbre->child[1];
    int res;
    while (execute_list(tempo))
    {
        for (int i = 0; i <= execution->nb_child - 1; i++)
        {
            res = check_type(execution->child[i]);
            if (res == -2) // car return de break
                break;
            if (res == -3) // car return de continue
                break;
            if (res == -5)
            {
                return -5;
            }
        }
        if (res == -2)
            break;
        if (res == -3)
            continue;
    }
    return 2;
}

int execute_or(struct ast *arbre)
{
    int res = 1;
    int fils = 0;
    if (arbre->child[0])
    {
        if (arbre->child[0]->type == AST_OR)
        {
            fils++;
            res = execute_or(arbre->child[0]);
        }
        if (arbre->child[0]->type == AST_AND)
        {
            fils++;
            res = execute_and(arbre->child[0]);
        }
        if (res == 0)
        {
            return 0;
        }
    }
    int execution = 1;
    while (execution != -1 && fils != 2)
    {
        int res = execute_command(arbre->child[fils]);
        if (res == -5)
        {
            return -5;
        }
        if (!res)
        {
            execution--;
            fils++;
        }
        else
        {
            execution = 1;
            break;
        }
    }
    if (execution == -1)
    {
        return 1;
    }
    return 0;
}

int execute_pip(struct ast *arbre)
{
    int fd[2];
    pid_t pid;

    // Create the pipe
    if (pipe(fd) == -1)
    {
        fprintf(stderr, "fail pipe");
        return 2;
    }

    // Fork the first child process
    pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "fail pid");
        return 2;
    }
    if (pid == 0)
    {
        // This is the first child process
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        int res = execute_command(arbre->child[0]);
        if (res == -5)
        {
            return -5;
        }
    }
    pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "fail pid");
        return 2;
    }
    if (pid == 0)
    {
        // This is the second child process
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        int res = execute_command(arbre->child[1]);
        if (res == -5)
        {
            return -5;
        }
    }
    // Close the pipe ends in the parent process
    close(fd[0]);
    close(fd[1]);

    // Wait for the child processes to finish
    wait(NULL);
    wait(NULL);

    return 0;
}

int do_classique(char *command)
{
    char **token = calloc(160, 1);
    lexer(command, token, NULL);
    if (strstr(token[0], "/bin/") == NULL
        || strstr(token[0], "/bin/") != token[0])
    {
        char *save = calloc(strlen(token[0]) + 6, 1);
        strcat(save, "/bin/");
        strcat(save, token[0]);
        free(token[0]);
        token[0] = save;
    }
    int ret = execv(token[0], token);
    free_token(token, 0);
    free(token);
    if (ret == -1)
        return 127;
    return ret;
}

int execute_builtin2(struct ast *arbre, char *command)
{
    char *res;
    if ((res = strstr(command, "./")) != NULL)
    {
        res += 2;
        command = res;
        char *argv[3];
        argv[0] = "42sh";
        argv[1] = command;
        argv[2] = NULL;
        if (access(command, F_OK) == 0)
        {
            global_var.retour = execv("./42sh", argv);
        }
        else
        {
            fprintf(stderr, "file does not exist\n");
            global_var.retour = 127;
        }
        return global_var.retour;
    }
    else if ((res = strstr(command, "cd ")) != NULL)
    {
        res += 3;
        command = res;
        int tempo;
        tempo = chdir(command);
        if (tempo == -1)
            fprintf(stderr, "path does not exist\n");
        return tempo;
    }
    int ret = is_custom_fct(arbre);
    if (ret != -1)
    {
        return 1;
    }
    else
    {
        global_var.retour = do_classique(command);
        if (global_var.retour == 127)
            return 127;
        return 1;
    }
}

int execute_builtin(struct ast *arbre, char *command)
{
    char *res;
    if ((res = strstr(command, "echo ")) != NULL)
    {
        res += 5;
        command = res;
        global_var.retour = 0;
        return echo(command);
    }
    else if (strcmp(command, "echo") == 0)
    {
        global_var.retour = 0;
        return 0;
    }

    else if (strcmp(command, "true") == 0)
    {
        global_var.retour = 0;
        return true();
    }
    else if (strcmp(command, "false") == 0)
    {
        global_var.retour = 1;
        return false();
    }
    else if (strcmp(command, "continue") == 0)
    {
        global_var.retour = 0;

        return -3;
    }
    else if (strcmp(command, "break") == 0)
    {
        global_var.retour = 0;
        return -2;
    }
    else if (strstr(command, "exit") == command)
    {
        my_exit(command + 4, &global_var.retour);
        return -5;
    }
    return execute_builtin2(arbre, command);
}

int execute_command(struct ast *arbre)
{
    if (!arbre)
        return -1;
    char *command = arbre->node;
    if (strcmp(command, "&&") == 0 && arbre->type == AST_AND)
    {
        int res = execute_and(arbre);
        if (res == 1)
            return false();
        else
            return true();
    }
    else if (strcmp(command, "||") == 0 && arbre->type == AST_OR)
    {
        int res = execute_or(arbre);
        if (res == 1)
            return false();
        else
            return true();
    }
    else if (strstr(command, "negation") != NULL && arbre->type == AST_NEG)
    {
        int res = execute_list(arbre);
        if (res == 1)
            return false();
        else
            return true();
    }
    else if (strcmp(command, "pipe") == 0 && arbre->type == AST_PIP)
    {
        return execute_pip(arbre);
    }
    else if (strcmp(command, "if") == 0 && arbre->type == AST_IF)
    {
        return execute_if(arbre);
    }
    else if (strcmp(command, "for") == 0 && arbre->type == AST_FOR)
    {
        return execute_for(arbre);
    }
    else if (strcmp(command, "while") == 0 && arbre->type == AST_WIL)
    {
        return execute_while(arbre);
    }
    else if (strcmp(command, "until") == 0 && arbre->type == AST_UTL)
    {
        return execute_until(arbre);
    }
    return execute_builtin(arbre, command);
}

int execute_sub(struct ast *arbre)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "fail pid");
        return 2;
    }
    if (pid == 0)
    {
        if (arbre->child)
        {
            execute_list(arbre);
        }
        else
        {
            fprintf(stderr, "missing end of parantheses");
            return 2;
        }
    }
    wait(NULL);
    return 1;
}
int execute_until(struct ast *arbre)
{
    if (arbre->nb_child != 2)
    {
        fprintf(stderr, "Missing arguments");
        return 2;
    }
    struct ast *tempo = arbre->child[0];

    struct ast *execution = arbre->child[1];
    int res;
    while (execute_list(tempo) == 0)
    {
        for (int i = 0; i <= execution->nb_child - 1; i++)
        {
            res = check_type(execution->child[i]);
            if (res == -2) // car return de break
                break;
            if (res == -3) // car return de continue
                break;
            if (res == -5)
            {
                return -5;
            }
        }
        if (res == -2)
            break;
        if (res == -3)
            continue;
    }
    return 1;
}

int execute_for(struct ast *arbre)
{
    char *chiffres = arbre->child[1]->node;
    long unsigned int index = 0;
    int *pointer = (int *)calloc(1, sizeof(int));
    long unsigned int i = 0;
    while (chiffres[index])
    {
        if (isdigit(chiffres[index]))
        {
            pointer[i] = atoi(&chiffres[index]);
            pointer = (int *)realloc(pointer, sizeof(pointer) + sizeof(int));
            i++;
        }
        index++;
    }
    for (long unsigned int j = 0; j < i; j++)
    {
        int res = execute_list(arbre->child[2]);
        if (res == -2) // car return de break
            break;
        if (res == -3) // car return de continue
            continue;
        if (res == -5) // received exit command, need to free
        {
            free(pointer);
            return -5;
        }
    }
    free(pointer);
    return 0;
}
