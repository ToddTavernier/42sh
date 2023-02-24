#include "parser.h"

#include "help_parser.h"
#include "shell_parser.h"
#include "tool_parser.h"

/*
input =
list '\n'
| list EOF
| '\n'
| EOF
;
list = and_or { ';' | '&' and_or } [ ';' | '&' ] ;
and_or = pipeline { '&&' | '||' {'\n'} pipeline } ;
pipeline = ['!'] command { '|' {'\n'} command } ;
command =
simple_command
| shell_command { redirection }
| funcdec { redirection }
;
simple_command =
prefix { prefix }
| { prefix } WORD { element }
;
shell_command =
'{' compound_list '}'
| '(' compound_list ')'
| rule_for
| rule_while
| rule_until
| rule_case
| rule_if
;
funcdec = WORD '(' ')' {'\n'} shell_command ;
redirection =
[IONUMBER] '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' WORD
| [IONUMBER] '<<' | '<<-' HEREDOC
;
prefix =
ASSIGNMENT_WORD
| redirection
;
element =
WORD
| redirection
;
compound_list =
{'\n'} and_or { ';' | '&' | '\n' {'\n'} and_or } [ ';' | '&' ] {'\n'} ;

rule_for =
'for' WORD [';'] | [ {'\n'} 'in' { WORD } ';' | '\n' ] {'\n'} 'do' compound_list
'done' ;

 rule_while = 'while' compound_list 'do' compound_list 'done' ;
rule_until = 'until' compound_list 'do' compound_list 'done' ;
rule_case = 'case' WORD {'\n'} 'in' {'\n'} [case_clause] 'esac' ;
rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
else_clause =
'else' compound_list
| 'elif' compound_list 'then' compound_list [else_clause]
;
case_clause = case_item { ';;' {'\n'} case_item } [';;'] {'\n'} ;
case_item = ['('] WORD { '|' WORD } ')' {'\n'} [compound_list] ;
*/

int is_redirection(char **list, struct ast *arbre)
{
    if (arbre == NULL)
    {
        return -1;
    }
    if (list[0] == NULL)
    {
        return -1;
    }
    int nb_token = 0;

    int nb = 0;
    if (list[0][0] <= '9' && list[0][0] >= '0')
    {
        nb += 1;
    }
    if (list[0][nb] == 0)
    {
        nb_token += 1;
        nb = 0;
    }
    if (list[nb_token] == NULL)
    {
        return -1;
    }
    if (list[nb_token][nb] == '>')
    {
        // strncat(ast_red->node, list[nb_token] + nb, 1);
        nb += 1;
        if (list[nb_token][nb] == '>' || list[nb_token][nb] == '&'
            || list[nb_token][nb] == '|')
        {
            // strcat(ast_red->node, list[nb_token][nb]);
            nb += 1;
        }
    }
    else if (list[nb_token][nb] == '<')
    {
        // strcat(ast_red->node, list[nb_token] + nb);
        nb += 1;
        if (list[nb_token][nb] == '>' || list[nb_token][nb] == '&')
        {
            // strcat(ast_red->node, list[nb_token][nb]);
            nb += 1;
        }
    }
    else
    {
        return -1;
    }
    if (list[nb_token][nb] == 0)
    {
        nb_token += 1;
        nb = 0;
    }
    // strcat(ast_red->node, list[nb_token] + nb);
    return nb_token;
}
// int is_fundec(char **list, struct ast *arbre)
// {
//     // int nb = 0;
//     return -1;
// }
int is_fundec(char **list, struct ast *arbre)
{
    int nb = 0;
    int ret = is_word(list, arbre, 0);
    if (ret == -1)
    {
        return -1;
    }
    nb++;
    if (list[nb] != NULL && list[nb + 1] != NULL && strcmp(list[nb], "(") == 0
        && strcmp(list[nb + 1], ")") == 0)
    {
        nb += 2;
        while (list[nb] != NULL && list[nb][0] == '\n')
        {
            nb++;
        }
        struct ast *ast_func = fill_node(NULL);
        ast_func->node = malloc(sizeof(char) * str_length(list[0]));
        strcpy(ast_func->node, list[0]);
        ast_func->type = AST_FNC;
        ret = is_shell_command(list + nb, ast_func);
        if (ret != -1)
        {
            nb += ret;
            add_child(arbre, ast_func);
            return nb;
        }
        free_ast(ast_func);
    }
    return -1;
}
int command(char **list, struct ast *arbre)
{
    // command =
    //      simple_command
    //      | shell_command { redirection }
    //      | funcdec { redirection }
    // ;

    // redirection for shell command pas fait
    int nb = 0;
    int ret = 0;
    ret = is_fundec(list, arbre);
    if (ret != -1)
    {
        return ret;
    }
    ret = is_shell_command(list, arbre);
    if (ret != -1)
    {
        while (ret != -1)
        {
            nb += ret;
            ret = is_redirection(list + nb, arbre);
        }
        return nb;
    }
    ret = simple_command(list, arbre);
    if (ret != -1)
    {
        return ret;
    }

    return -1;
}
int pipeline(char **list, struct ast *arbre)
{
    int nb = 0;
    // pipeline = ['!'] command { '|' {'\n'} command } ;
    if (list[0] != NULL && strcmp(list[0], "!") == 0)
    {
        // negation
        struct ast *ast_neg = fill_node(arbre);
        ast_neg->node = malloc(sizeof(char) * 20);
        strcpy(ast_neg->node, "negation");
        ast_neg->type = AST_NEG;
        arbre = ast_neg;
        nb++;
        if (list[nb] == NULL)
        {
            return nb;
        }
    }
    int ret = command(list + nb, arbre);
    if (ret == -1)
    {
        return -1;
    }
    nb += ret;

    while (list[nb] != NULL && strcmp(list[nb], "|") == 0)
    {
        nb += 1; // create pipenode and make it the father of previous cmd node
        struct ast *ast_pip = fill_node(NULL);
        add_child(ast_pip, arbre->child[arbre->nb_child - 1]);
        arbre->child[arbre->nb_child - 1] = ast_pip;
        ast_pip->node = malloc(sizeof(char) * 6);

        strcpy(ast_pip->node, "pipe");
        ast_pip->type = AST_PIP;
        ret = command(list + nb, ast_pip);
        if (ret == -1)
        {
            // input probably wrong, free everything
            return -1;
        }
        nb += ret;
    }
    return nb;
}
int is_and_or(char **list, struct ast *arbre)
{
    int ret = pipeline(list, arbre);
    if (ret == -1)
    {
        return -1;
    };
    while (list[ret] != NULL
           && (strcmp(list[ret], "||") == 0 || strcmp(list[ret], "&&") == 0))
    {
        // create and_or node and make it the father of previous cmd node
        struct ast *ast_and = fill_node(NULL);
        add_child(ast_and, arbre->child[arbre->nb_child - 1]);
        arbre->child[arbre->nb_child - 1] = ast_and;
        ast_and->node = malloc(sizeof(char) * 3);
        strcpy(ast_and->node, list[ret]);
        if (strcmp(list[ret], "||") == 0)
        {
            ast_and->type = AST_OR;
        }
        else
        {
            ast_and->type = AST_AND;
        }
        ret += 1;
        int nb = pipeline(list + ret, ast_and);
        if (nb == -1)
        {
            // input probably wrong, free everything
            return -1;
        }
        ret += nb;
    }
    return ret;
}
int is_list(char **list, struct ast *arbre)
{
    int nb = is_and_or(list, arbre);
    if (nb == -1)
    {
        return -1;
    }
    if (list[nb] == NULL)
    {
        return nb;
    }
    if (list[nb][0] == ';')
    {
        nb += 1;
    }
    else
    {
        return nb;
    }
    if (list[nb] != NULL)
    {
        int ret = is_list(list + nb, arbre);
        if (ret == -1)
        {
            ret = 0;
        }
        return nb + ret;
    }
    return nb;
}
int is_input(char **list, struct ast *arbre)
{
    int nb = 0;
    if (list[0][0] == EOF)
    {
        return 1;
    }
    else if (list[0][0] == '\n')
    {
        return 1;
    }
    nb = is_list(list, arbre);
    if (nb == -1)
    {
        return -1;
    }
    if (list[nb] == NULL)
    {
        return nb;
    }
    if (list[nb][0] == EOF)
    {
        return nb + 1;
    }
    if (list[nb][0] == '\n')
    {
        return nb + 1;
    }
    return -1;
}
int parser(char **list, struct ast *arbre)
{
    int nb = 0;
    while (list[nb] != NULL)
    {
        int ret = is_input(list + nb, arbre);
        if (ret == -1)
        {
            return -1;
        }
        nb += ret;
    }
    return 0;
}
