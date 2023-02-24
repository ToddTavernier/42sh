#ifndef NEW_LEXER_H
#define NEW_LEXER_H

#include "evalxpr/evalxpr.h"
#include "evalxpr/fifo.h"
#include "include.h"

struct variables
{
    char *name;
    char *value;
    struct variables *next;
};

char char_esc_b(int *escape);
char check_char_esc(char c, int *escape);
char *escape_char_str(char *str, int *str_index, int *escape,
                      char *classique_token);
                      char *lex_variable(char *str, int *open_char, char *argv[],
                   struct variables *vars);
char *switch_UID(void);
char *switch_at(int count_arg, char **argv);
char *switch_star(int count_arg, char **argv);
char *switch_variable(char *a, char **argv);
void change_var(char *unset_var, struct variables *vars);
char *eval_inter(char *a);

#endif /* !NEW_LEXER_H */
