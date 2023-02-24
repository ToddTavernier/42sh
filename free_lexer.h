#ifndef FREE_LEXER_H
#define FREE_LEXER_H

#include "include.h"
#include "lexer.h"

void free_token(char **list, size_t i);
void free_vars(struct variables *vars);

#endif /* !FREE_LEXER_H */
