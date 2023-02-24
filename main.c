#include "include.h"
#include "lexer.h"
#include "parcours.h"
#include "parser.h"

void free_ressources(struct ast *arbre, char **token)
{
    free_ast(arbre);
    free_token(token, 0);
    free(token);
}

// transformer texte d'un file.sh en une string à utiliser pour le lexer
char *read_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return NULL;
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    char *content = malloc(size + 1);
    if (content == NULL)
    {
        fclose(fp);
        return NULL;
    }
    fread(content, size, 1, fp);
    content[size] = '\0';
    fclose(fp);
    return content;
}

struct ast *init_ast(void)
{
    struct ast *arbre = fill_node(NULL);
    arbre->node = malloc(sizeof(char) * 20);
    strcpy(arbre->node, "root");
    return arbre;
}

char *open_file(char *filename, char **token)
{
    char *input = read_file(filename);
    if (input == NULL)
    {
        fprintf(stderr, "Erreur lors de la lecture du fichier %s\n", filename);
        free(token);
        return NULL;
    }
    return input;
}

int check_token(int nb_token, char **token)
{
    if (nb_token <= 0 || token[0][0] == '\0')
    {
        if (nb_token <= -1 || token[0][0] == '\0')
        {
            if (!token[0] || (token[0] && token[0][0] == '\0'))
                fprintf(stderr, "Empty command\n");
            else
                fprintf(stderr, "missing end of quote\n");

            if (!token[0] || (token[0] && token[0][0] == '\0')
                || nb_token == -2)
            {
                free_token(token, 0);
                free(token);
                return 2;
            }
            free_token(token, 0);
            free(token);
            return 1;
        }
        free_token(token, 0);
        free(token);
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    int nb_token = 0;
    int is_input = 0;
    char **token = calloc(1024, 16);
    char *input;
    if (argc == 3 && strcmp(argv[1], "-c") == 0) // ./42sh -c "echo exemple"
        input = argv[2];
    else if (argc == 2) // ./42sh exemple.sh
    {
        if (!(input = open_file(argv[1], token)))
            return 1;
    }
    else if (argc == 1) // ./42sh (lecture stdin)
    {
        input = calloc(10300, 1);
        read(0, input, 10200);
        is_input = 1;
    }
    else
    {
        fprintf(stderr, "Usage: ./42sh {-c} {file} {'script'}\n");
        free(token);
        return 2;
    }
    nb_token = lexer(input, token, argv);
    if (argc != 3)
        free(input);
    int res = 0;
    if ((res = check_token(nb_token, token)) != -1)
        return res;
    struct ast *arbre = init_ast();
    if (parser(token, arbre) == -1)
    {
        fprintf(stderr, "Parsing Error\n");
        free_ressources(arbre, token);
        return 2;
    }
    int return_value = 0;
    if (parcours(arbre, is_input, &return_value) != 0)
    {
        if (is_input == 0)
            fprintf(stderr, "Fail in the execution of the script\n");
        free_ressources(arbre, token);
        if (is_input == 1 && return_value != 127)
            return 0;
        return return_value;
    }
    free_ressources(arbre, token);
    return return_value;
}
