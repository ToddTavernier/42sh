#ifndef PARCOURS_H
#define PARCOURS_H
#include "parser.h"

struct name_def
{
    char *name;
    struct ast *arbre;
};
struct functions
{
    int nb_function;
    int nb_allocated;
    struct name_def **array;
};
struct global_var
{
    struct functions functions;
    int retour;
};

int parcours(struct ast *arbre, int is_input, int *return_value);

int execute_list(struct ast *arbre);
int execute_and(struct ast *arbre);
int check_type(struct ast *arbre);
int execute_if(struct ast *arbre);
int execute_function(struct ast *arbre);
int is_custom_fct(struct ast *arbre);
void free_functions(void);

#endif /* !PARCOURS_H */
