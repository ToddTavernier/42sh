#ifndef LEXER3_H
#define LEXER3_H

#include "include.h"
#include "lexer.h"
#include "new_lexer.h"

struct var_lexer
{
    size_t *token_index;
    size_t *single_quote;
    size_t *double_quote;
    size_t *escape_char;
    size_t *var_set;
    int *str_index;
    int *nb_char_open;
    char **argv;
    int *my_continue;
};

int retour_error(char *classique_token);
char *token_is_unset(char *str, struct variables *vars);
int size_str(char *str);
int islimit(char c, size_t single_quote, size_t double_quote);
char *add_token(char **token, size_t *token_index, size_t taille, char *str);
char *str_EOL(size_t *var_set, char **token, size_t *token_index, char *str);
void realloc_token(char **token, size_t *token_index, size_t taille, char *str);
char *help_lex_dollar(char *str, char **token, struct var_lexer *var_lex);
char *lex_dollar(char *str, char **token, struct var_lexer *var_lex,
                 struct variables *vars);
void free_var_lexer(struct var_lexer *var_lex);

#endif /* !LEXER3_H */
