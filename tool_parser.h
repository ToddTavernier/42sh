#ifndef TOOL_PARSER_H
#define TOOL_PARSER_H

#include "include.h"
#include "parser.h"

void remove_first_char(char *str);
struct ast *get_last_char(struct ast *arbre);
int sizeof_str(char *str);
int str_length(char *str);
struct ast *fill_node(struct ast *arbre);
int add_child(struct ast *arbre, struct ast *child);
int exist_in_if_grammar(char *elt);
int exist_in(char *elt);
void free_ast(struct ast *arbre);

#endif /* !TOOL_PARSER_H */
