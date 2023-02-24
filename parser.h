#ifndef PARSER_H
#define PARSER_H

#include "include.h"

enum ast_type
{
    AST_CMD,
    AST_IF,
    AST_LIST,
    AST_WORD,
    AST_RED,
    AST_PIP,
    AST_NEG,
    AST_WIL,
    AST_UTL,
    AST_AND,
    AST_FOR,
    AST_OR,
    AST_SUB,
    AST_FNC
};

struct ast
{
    char *node;
    int nb_child;
    int capacity;
    struct ast **child;
    enum ast_type type;
};

int parser(char **list, struct ast *arbre);
struct ast *fill_node(struct ast *arbre);
void free_ast(struct ast *arbre);
int is_redirection(char **list, struct ast *arbre);
int is_fundec(char **list, struct ast *arbre);
int command(char **list, struct ast *arbre);
int pipeline(char **list, struct ast *arbre);
int is_and_or(char **list, struct ast *arbre);
int is_list(char **list, struct ast *arbre);
int is_input(char **list, struct ast *arbre);

#endif /* !PARSER_H */
