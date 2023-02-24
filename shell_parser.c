
#include "help_parser.h"
#include "parser.h"
#include "tool_parser.h"

int is_else_clause(char **list, struct ast *arbre)
{
    //  else_clause =
    // 'else' compound_list
    // | 'elif' compound_list 'then' compound_list [else_clause]
    // ;

    int nb = 0;
    if (list[nb] == NULL)
    {
        return -1;
    }
    if (strcmp(list[0], "else") == 0)
    {
        nb += 1;
        int ret = is_compound_list(list + nb, arbre);
        return ret + nb;
    }
    else if (strcmp(list[0], "elif") == 0)
    {
        struct ast *ast_ret = fill_node(arbre);
        ast_ret->node = malloc(sizeof(char) * 20);
        strcpy(ast_ret->node, "if");
        ast_ret->type = AST_IF;

        struct ast *ast_condition = fill_node(ast_ret);
        ast_condition->node = malloc(sizeof(char) * 20);
        strcpy(ast_condition->node, "condition");
        ast_condition->type = AST_LIST;

        int ret = is_compound_list(list + nb, arbre);
        if (strcmp(list[nb], "then"))
        {
            return -1;
        }
        nb += 1;
        struct ast *ast_then = fill_node(ast_ret);
        ast_then->node = malloc(sizeof(char) * 20);
        strcpy(ast_then->node, "then");
        ast_then->type = AST_LIST;
        ret = is_compound_list(list + nb, arbre);
        if (ret == -1)
        {
            return -1;
        }
        struct ast *ast_else = fill_node(ast_ret);
        ast_else->node = malloc(sizeof(char) * 20);
        strcpy(ast_else->node, "else");
        ast_else->type = AST_LIST;
        ret = is_else_clause(list + nb, ast_else);
        if (ret != -1)
        {
            nb += ret;
        }
        return nb;
    }

    return -1;
}
int is_rule_until(char **list, struct ast *arbre)
{
    // rule_until = 'until' compound_list 'do' compound_list 'done' ;
    int nb = 0;
    if (strcmp(list[0], "until"))
    {
        return -1;
    }
    struct ast *ast_ret = fill_node(arbre);
    ast_ret->node = malloc(sizeof(char) * 20);
    strcpy(ast_ret->node, "until");
    ast_ret->type = AST_UTL;
    nb += 1;
    // create list for if condition and fill it with childrens line below
    // first child is a node named condition whose children will be: true;
    // false;...
    struct ast *ast_condition = fill_node(ast_ret);
    ast_condition->node = malloc(sizeof(char) * 20);
    strcpy(ast_condition->node, "condition");
    ast_condition->type = AST_LIST;
    int ret = is_compound_list(list + nb, ast_condition);
    if (ret == -1)
    {
        return ret;
    }
    nb += ret;
    if (strcmp(list[nb], "do"))
    {
        return -1;
    }
    nb += 1;
    struct ast *ast_then = fill_node(ast_ret);
    ast_then->node = malloc(sizeof(char) * 20);
    strcpy(ast_then->node, "do");
    ast_then->type = AST_LIST;
    ret = is_compound_list(list + nb, ast_then);
    if (ret == -1)
    {
        return ret;
    }
    nb += ret;
    if (list[nb] == NULL || strcmp(list[nb], "done"))
    {
        return -1;
    }
    return nb + 1;
}
int is_rule_while(char **list, struct ast *arbre)
{
    // rule_while = 'while' compound_list 'do' compound_list 'done' ;
    int nb = 0;
    if (strcmp(list[0], "while"))
    {
        return -1;
    }
    struct ast *ast_ret = fill_node(arbre);
    ast_ret->node = malloc(sizeof(char) * 20);
    strcpy(ast_ret->node, "while");
    ast_ret->type = AST_WIL;
    nb += 1;
    // create list for if condition and fill it with childrens line below
    // first child is a node named condition whose children will be: true;
    // false;...
    struct ast *ast_condition = fill_node(ast_ret);
    ast_condition->node = malloc(sizeof(char) * 20);
    strcpy(ast_condition->node, "condition");
    ast_condition->type = AST_LIST;
    int ret = is_compound_list(list + nb, ast_condition);
    if (ret == -1)
    {
        return ret;
    }
    nb += ret;
    if (strcmp(list[nb], "do"))
    {
        return -1;
    }
    nb += 1;
    struct ast *ast_then = fill_node(ast_ret);
    ast_then->node = malloc(sizeof(char) * 20);
    strcpy(ast_then->node, "do");
    ast_then->type = AST_LIST;
    ret = is_compound_list(list + nb, ast_then);
    if (ret == -1)
    {
        return ret;
    }
    nb += ret;
    if (list[nb] == NULL || strcmp(list[nb], "done"))
    {
        return -1;
    }
    return nb + 1;
}
struct ast *create_ast(struct ast *arbre, char *name, enum ast_type type)
{
    struct ast *ast_for = fill_node(arbre);
    ast_for->node = malloc(sizeof(char) * str_length(name));
    strcpy(ast_for->node, name);
    ast_for->type = type;
    return ast_for;
}
int is_just_do(char **list, struct ast *ast_for)
{
    int nb = 0;
    int ret = 0;
    if (strcmp(list[nb], "do"))
        return -1;

    nb += 1;
    struct ast *ast_do = create_ast(ast_for, "do", AST_LIST);
    ret = is_compound_list(list + nb, ast_do);
    if (ret == -1)
        return ret;

    nb += ret;
    if (list[nb] == NULL || strcmp(list[nb], "done"))
        return -1;

    nb++;
    return nb;
}
int is_rule_for_helper(int nb, char **list, struct ast *ast_for)
{
    int ret = 0;
    struct ast *ast_ensemble = create_ast(ast_for, list[nb], AST_WORD);
    nb++;
    while (list[nb] != NULL && ret != -1)
    {
        ret = is_word(list + nb, ast_ensemble, 1);
        if (ret != -1)
        {
            ast_ensemble->node = realloc(ast_ensemble->node,
                                         sizeof(char)
                                             * (str_length(ast_ensemble->node)
                                                + str_length(list[nb]) + 1));
            strcat(ast_ensemble->node, " ");
            strcat(ast_ensemble->node, list[nb]);
        }
        nb++;
    }
    return nb;
}
int is_rule_for(char **list, struct ast *arbre)
{
    // rule_for = 'for' WORD[';'] |
    //     [{ '\n' } 'in' { WORD } ';' | '\n'] { '\n' } 'do' compound_list
    //     'done';
    int nb = 0;
    if (strcmp(list[0], "for"))
        return -1;
    struct ast *ast_for = create_ast(arbre, "for", AST_FOR);
    nb += 1;
    int ret = is_word(list + nb, ast_for, 1);
    if (ret == -1)
        return -1;

    struct ast *ast_var = create_ast(ast_for, list[nb], AST_WORD);
    nb++;
    if (strcmp(list[nb], ";") == 0)
        nb += 1;

    else
    {
        while (list[nb][0] == '\n')
            nb++;

        if (strcmp(list[nb], "in") != 0)
        {
            ret = is_just_do(list + nb, ast_for);
            if (ret == -1)
            {
                return -1;
            }
            return nb + ret;
        }
        nb++;
        ret = is_word(list + nb, ast_var, 1);
        if (ret != -1)
        {
            nb = is_rule_for_helper(nb, list, ast_for);
        }
    }
    while (list[nb][0] == '\n')
        nb++;

    ret = is_just_do(list + nb, ast_for);
    if (ret == -1)
        return -1;
    return nb + ret;
}

int is_rule_if(char **list, struct ast *arbre)
{
    int nb = 0;
    if (strcmp(list[0], "if"))
    {
        return -1;
    }
    struct ast *ast_ret = fill_node(arbre);
    ast_ret->node = malloc(sizeof(char) * 20);
    strcpy(ast_ret->node, "if");
    ast_ret->type = AST_IF;
    nb += 1;
    // create list for if condition and fill it with childrens line below
    // first child is a node named condition whose children will be: true;
    // false;...
    struct ast *ast_condition = create_ast(ast_ret, "condition", AST_LIST);
    int ret = is_compound_list(list + nb, ast_condition);
    if (ret == -1)
    {
        return ret;
    }
    nb += ret;
    if (list[nb] == NULL || strcmp(list[nb], "then"))
    {
        return -1;
    }
    nb += 1;
    struct ast *ast_then = create_ast(ast_ret, "then", AST_LIST);
    ret = is_compound_list(list + nb, ast_then);
    if (ret == -1)
    {
        return ret;
    }
    nb += ret;
    struct ast *ast_else = create_ast(ast_ret, "else", AST_LIST);
    ast_else->node = malloc(sizeof(char) * 20);
    strcpy(ast_else->node, "else");
    ast_else->type = AST_LIST;
    ret = is_else_clause(list + nb, ast_else);
    if (ret != -1)
    {
        nb += ret;
    }
    if (list[nb] == NULL || strcmp(list[nb], "fi"))
    {
        return -1;
    }
    return nb + 1;
}

int is_shell_command(char **list, struct ast *arbre)
{
    //     shell_command =
    // '{' compound_list '}'
    // | '(' compound_list ')'
    // | rule_for
    // | rule_while
    // | rule_until
    // | rule_case
    // | rule_if
    // ;
    int nb = 0;
    int ret = 0;
    if (list[nb] != NULL && strcmp(list[nb], "(") == 0)
    {
        nb++;
        struct ast *ast_sub = create_ast(NULL, "subshell", AST_SUB);
        ret = is_compound_list(list + nb, ast_sub);
        if (ret != -1)
        {
            if (strcmp(list[nb + ret], ")") == 0)
            {
                nb = nb + ret + 1;
                add_child(arbre, ast_sub);
                return nb;
            }
        }
        free_ast(ast_sub);
    }
    if (list[nb] != NULL && strcmp(list[nb], "{") == 0)
    {
        nb++;
        struct ast *ast_func = create_ast(NULL, "Command block", AST_LIST);
        ret = is_compound_list(list + nb, ast_func);
        if (ret != -1)
        {
            if (strcmp(list[nb + ret], "}") == 0)
            {
                nb += ret + 1;
                add_child(arbre, ast_func);
                return nb;
            }
        }
        free_ast(ast_func);
    }
    ret = is_rule_if(list, arbre);
    if (ret != -1)
    {
        return ret;
    }
    ret = is_rule_while(list, arbre);
    if (ret != -1)
    {
        return ret;
    }
    ret = is_rule_until(list, arbre);
    if (ret != -1)
    {
        return ret;
    }
    ret = is_rule_for(list, arbre);
    if (ret != -1)
    {
        return ret;
    }
    return -1;
}
