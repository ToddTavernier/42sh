
#include "parser.h"
#include "shell_parser.h"
#include "tool_parser.h"

int is_word(char **list, struct ast *arbre, int command)
{
    int nb = 0;
    int dbquote = 0;
    if (arbre == NULL)
    {
        dbquote = 0;
    }
    if (list[nb] == NULL)
    {
        return -1;
    }
    if (list[nb][0] == '\"')
    {
        nb++;
        dbquote = 1;
        return 0;
    }
    if (list[nb][0] == '\'')
    {
        nb++;
    }
    if (command == 0)
    {
        // when we don't want the word to be stuff like: for,if,then...
        if (exist_in_if_grammar(list[nb]))
        {
            return -1;
        }
    }
    if (exist_in(list[nb]))
    {
        return -1;
    }
    return nb - dbquote;
}
int is_assignement_word(char **list, struct ast *arbre)
{
    if (arbre == NULL)
    {
        return -1;
    }
    if (list[0] != NULL && list[0][0] == '$')
    {
        // expand variable
        switch (list[0][1])
        {
        case '1':
            strcpy(list[0], "was $1");
            break;

        default:
            break;
        }
    }
    return -1;
}
int is_prefix(char **list, struct ast *arbre)
{
    int ret = is_assignement_word(list, arbre);
    if (ret != -1)
    {
        return ret;
    }
    return is_redirection(list, arbre);
}
int is_element(char **list, struct ast *arbre, struct ast *ast_ret)
{
    int ret = is_redirection(list, arbre);
    if (ret != -1)
    {
        // is a redirection so create node and make it the father
        // it will take the place of the last child of arbre

        struct ast *ast_red = fill_node(NULL);
        ast_red->node = malloc(sizeof(char) * str_length(list[0]));
        strcpy(ast_red->node, list[0]);
        for (int i = 0; i < ret; i++)
        {
            if (list[1 + i] == NULL)
            {
                free_ast(ast_red);
                return -1;
            }
            ast_red->node = realloc(
                ast_red->node,
                sizeof(char)
                    * (str_length(ast_red->node) + str_length(list[i + 1])));
            strcat(ast_red->node, list[1 + i]);
        }
        ast_red->type = AST_RED;
        struct ast *previous_child = arbre->child[arbre->nb_child - 1];
        arbre->child[arbre->nb_child - 1] = ast_red;
        add_child(ast_red, previous_child);
        return ret + 1;
    }
    int nb = is_word(list, arbre, 1);
    if (nb == -1)
    {
        return -1;
    }
    if (strcmp(list[nb], "\"") == 0)
    {
        nb++;
    }
    ast_ret->node = realloc(
        ast_ret->node,
        sizeof(char) * (str_length(ast_ret->node) + str_length(list[nb]) + 1));

    strcat(ast_ret->node, " ");
    strcat(ast_ret->node, list[nb]);
    nb++;
    if (list[nb] != NULL && strcmp(list[nb], "\"") == 0)
    {
        nb++;
    }
    return nb;
}

int simple_command(char **list, struct ast *arbre)
{
    int nb = 0;

    // simple_command =
    //       prefix { prefix }  <-- pas fait
    //       | { prefix } WORD { element }
    //       ;

    //  |
    // check if is prefix
    int ret = is_prefix(list, arbre);
    if (ret != -1)
    {
        // create red node then recusive of simple command
        struct ast *ast_red = fill_node(arbre);
        ast_red->node = malloc(sizeof(char) * 20);
        strcpy(ast_red->node, list[0]);

        for (int i = 1; i < ret; i++)
        {
            strcat(ast_red->node, list[i]);
        }
        ast_red->type = AST_RED;
        nb += ret + 1;

        arbre = ast_red;
    }

    ret = is_word(list + nb, arbre, 0); // the command
    nb += ret;
    if (ret == -1)
    {
        return -1;
    }
    int dbquotes = 0;
    if (strcmp(list[nb], "\"") == 0)
    {
        dbquotes = 1;
        nb++;
    }
    struct ast *ast_ret = fill_node(arbre);
    ast_ret->node = malloc(sizeof(char) * str_length(list[nb]));
    strcpy(ast_ret->node, list[nb]);
    ast_ret->type = AST_CMD;
    nb++;
    if (dbquotes == 1 && list[nb] != NULL && strcmp(list[nb], "\"") == 0)
    {
        nb++;
    }
    ret = is_element(list + nb, arbre, ast_ret);
    while (ret != -1)
    {
        nb += ret;
        ret = is_element(list + nb, arbre, ast_ret);
    }
    return nb;
}
int is_compound_list(char **list, struct ast *arbre)
{
    // compound_list = {'\n'} and_or { ';' | '\n' {'\n'} and_or } [';'] {'\n'} ;
    int nb = 0;
    if (list[nb] == NULL)
    {
        return -1;
    }
    while (list[nb][0] == '\n')
    {
        nb += 1;
        if (list[nb] == NULL)
        {
            return -1;
        }
    }
    int ret = is_and_or(list + nb, arbre);
    if (ret == -1)
    {
        return -1;
    }
    nb += ret;
    int rec = 1;
    // need to know if we go there   |                          |
    //                               v                          v
    // compound_list = {'\n'} and_or { ';' | '\n' {'\n'} and_or } [';'] {'\n'} ;

    // so i check if we have ';' | '\n' first then we go recursivly
    if (list[nb] == NULL)
    {
        return -1;
    }
    if (list[nb][0] == ';' || list[nb][0] == '\n')
    {
        rec = 0;
        nb += 1;
    }
    if (list[nb] == NULL)
    {
        return nb;
    }
    if (rec == 0)
    {
        rec = is_compound_list(list + nb, arbre);
        if (rec != -1)
        {
            nb += rec;
        }
    }
    if (list[nb] == NULL)
    {
        return nb;
    }
    if (list[nb][0] == ';')
    {
        nb += 1;
    }
    while (list[nb] != NULL && list[nb][0] == '\n')
    {
        nb += 1;
    }
    return nb;
}
