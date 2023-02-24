#include "parcours.h"

#include "builtins/boolean.h"
#include "builtins/echo.h"
#include "include.h"
#include "lexer.h"
#include "parcours2.h"
#include "parser.h"
#include "redirection.h"
struct global_var global_var;

int execute_list(struct ast *arbre)
{
    int res = 0;
    for (int i = 0; i < arbre->nb_child; i++)
    {
        res = execute_command(arbre->child[i]);
        if (res == -2) // car return de break
            break;
        if (res == -3) // car return de continue
            break;
        if (res == -5)
        {
            return -5;
        }
    }
    if (res == -2) // car return de break
        return res;
    if (res == -3) // car return de continue
        return res;
    if (arbre->nb_child > 0 && !res)
        return 0;
    return 1;
}

int execute_if(struct ast *arbre)
{
    int res = 2;
    if (arbre->nb_child > 2)
    {
        if ((res = execute_list(arbre->child[0])))
            return execute_list(arbre->child[1]);
        if (arbre->child[2]->child[0])
        {
            if (arbre->child[2]->child[0]->type == AST_IF)
                return execute_if(arbre->child[2]->child[0]);
            return execute_list(arbre->child[2]);
        }
    }
    global_var.retour = 0;
    return res;
}

int execute_redirection(struct ast *arbre)
{
    if (arbre->type == AST_RED)
    {
        int index = 0;
        int left = 0;
        int right = 0;
        while (arbre->node[index])
        {
            if (arbre->node[index] == '>')
            {
                left = 1;
                break;
            }
            if (arbre->node[index] == '<')
            {
                right = 1;
                break;
            }
            index++;
        }
        if (left == 1)
            return to_left(arbre);
        else if (right == 1)
            return to_right(arbre);
    }
    return 2;
}

int execute_neg(struct ast *arbre)
{
    int res = execute_list(arbre);
    if (global_var.retour == 0)
    {
        global_var.retour = 1;
    }
    else if (global_var.retour == 1)
    {
        global_var.retour = 0;
    }
    return res;
}

int check_type(struct ast *arbre)
{
    if (!arbre->child)
        return 2;

    enum ast_type type = arbre->type;
    if (type == AST_IF)
        return execute_if(arbre);
    if (type == AST_CMD)
        return execute_command(arbre);
    if (type == AST_RED)
        return execute_redirection(arbre);
    if (type == AST_WIL)
        return execute_while(arbre);
    if (type == AST_UTL)
        return execute_until(arbre);
    if (type == AST_FOR)
        return execute_for(arbre);
    if (type == AST_AND)
        return execute_and(arbre);
    if (type == AST_OR)
        return execute_or(arbre);
    if (type == AST_NEG)
        return execute_neg(arbre);
    if (type == AST_PIP)
        return execute_pip(arbre);
    if (type == AST_SUB)
    {
        return execute_sub(arbre);
    }
    if (type == AST_FNC)
        return execute_function(arbre);
    return 1;
}

int execute_and(struct ast *arbre)
{
    int res = 0;
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
    }
    if (res == 1)
    {
        return 1;
    }
    int execution = 1;
    while (execution && fils != 2)
    {
        int res = execute_command(arbre->child[fils]);
        if (res == -5)
        {
            return -5;
        }
        if (!res)
        {
            execution = 0;
            break;
        }
        fils++;
    }
    if (execution == 0)
    {
        return 1;
    }
    return 0;
}

void free_functions(void)
{
    for (int i = 0; i < global_var.functions.nb_function; i++)
    {
        free(global_var.functions.array[i]->name);
        free(global_var.functions.array[i]);
    }
    free(global_var.functions.array);
}
int execute_function(struct ast *arbre)
{
    struct name_def *function = malloc(sizeof(struct name_def));
    function->name = malloc(sizeof(char) * (strlen(arbre->node) + 1));
    strcpy(function->name, arbre->node);
    function->arbre = arbre->child[0];
    if (global_var.functions.nb_function == global_var.functions.nb_allocated)
    {
        global_var.functions.nb_allocated *= 2;
        global_var.functions.nb_allocated++;

        global_var.functions.array = realloc(global_var.functions.array,
                                             global_var.functions.nb_allocated
                                                 * sizeof(struct name_def));
    }
    global_var.functions.array[global_var.functions.nb_function] = function;
    global_var.functions.nb_function++;
    return 1;
}
int is_custom_fct(struct ast *arbre)
{
    for (int i = 0; i < global_var.functions.nb_function; i++)
    {
        if (strcmp(global_var.functions.array[i]->name, arbre->node) == 0)
        {
            if (global_var.functions.array[i]->arbre->type == AST_LIST)
            {
                return execute_list(global_var.functions.array[i]->arbre);
            }
            else
            {
                return check_type(global_var.functions.array[i]->arbre);
            }
        }
    }
    return -1;
}

int parcours(struct ast *arbre, int is_input, int *return_value)
{
    global_var.retour = 0;

    global_var.functions.nb_allocated = 0;
    global_var.functions.nb_function = 0;
    global_var.functions.array = NULL;
    if (!arbre)
        return 1;
    int i = 0;
    int res = 0;
    for (i = 0; i < arbre->nb_child; i++)
    {
        if ((res = check_type(arbre->child[i])) != 1 && is_input == 0)
        {
            *return_value = res;
            break;
        }
        if (res == -5)
        {
            break;
        }
    }
    *return_value = global_var.retour;
    if (res == -5)
    {
        return 0;
    }
    if (i != arbre->nb_child)
        return res;
    return 0;
}
