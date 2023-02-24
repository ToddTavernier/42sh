#ifndef HELP_PARSER_H
#define HELP_PARSER_H

#include "include.h"
#include "parser.h"

int is_compound_list(char **list, struct ast *arbre);
int simple_command(char **list, struct ast *arbre);
int is_element(char **list, struct ast *arbre, struct ast *ast_ret);
int is_prefix(char **list, struct ast *arbre);
int is_assignement_word(char **list, struct ast *arbre);
int is_word(char **list, struct ast *arbre, int command);

#endif /* !HELP_PARSER_H */
