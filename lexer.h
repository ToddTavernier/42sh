#ifndef LEXER_H
#define LEXER_H

#include "include.h"
#include "lexer3.h"
#include "new_lexer.h"

void free_vars(struct variables *vars);
void free_token(char **list, size_t i);
size_t lexer(char *str, char **token, char *argv[]);
char *lex_variable(char *str, int *open_char, char *argv[],
                   struct variables *vars);

#endif /* !LEXER_H */
