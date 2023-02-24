#include "free_lexer.h"

void free_vars(struct variables *vars)
{
    if (!vars)
        return;
    free_vars(vars->next);
    free(vars->name);
    free(vars->value);
    free(vars);
}

void free_token(char **list, size_t i)
{
    if (list[i] == NULL)
        return;
    free_token(list, i + 1);
    free(list[i]);
}
