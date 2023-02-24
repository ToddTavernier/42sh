#ifndef PARCOURS2_H
#define PARCOURS2_H

#include "parser.h"

int execute_command(struct ast *arbre);
int execute_sub(struct ast *arbre);
int execute_for(struct ast *arbre);
int execute_until(struct ast *arbre);
int execute_while(struct ast *arbre);
int execute_or(struct ast *arbre);
int execute_pip(struct ast *arbre);

#endif /* !PARCOURS2_H */
