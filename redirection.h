#ifndef REDIRECTION_H
#define REDIRECTION_H
#include "builtins/boolean.h"
#include "builtins/echo.h"
#include "include.h"
#include "parcours.h"
#include "parcours2.h"
#include "parser.h"

int to_left(struct ast *arbre);
int to_right(struct ast *arbre);
int write_over(struct ast *arbre, int fd, int fd_dup, char *autorisation);

#endif
