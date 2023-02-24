
#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include "include.h"
#include "parser.h"

int is_shell_command(char **list, struct ast *arbre);
int is_rule_if(char **list, struct ast *arbre);
int is_rule_for(char **list, struct ast *arbre);
int is_rule_for_helper(int nb, char **list, struct ast *ast_for);
int is_just_do(char **list, struct ast *ast_for);
struct ast *create_ast(struct ast *arbre, char *name, enum ast_type type);
int is_rule_while(char **list, struct ast *arbre);
int is_rule_until(char **list, struct ast *arbre);
int is_else_clause(char **list, struct ast *arbre);

#endif /* !SHELL_PARSER_H */
