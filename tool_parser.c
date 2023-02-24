
#include "help_parser.h"
#include "parser.h"
#include "shell_parser.h"

void remove_first_char(char *str)
{
    int i = 0;
    while (str[i] != 0)
    {
        str[i] = str[i + 1];
        i++;
    }
}
struct ast *get_last_char(struct ast *arbre)
{
    if (arbre->nb_child == 0)
    {
        return arbre;
    }
    return get_last_char(arbre->child[arbre->nb_child - 1]);
}
int sizeof_str(char *str)
{
    int i = 0;
    while (str[i] != 0)
    {
        i++;
    }
    return i - 1;
}
int str_length(char *str)
{
    if (str == NULL)
    {
        return -1;
    }
    // initializing count variable (stores the length of the string)
    int count;

    // incrementing the count till the end of the string
    for (count = 0; str[count] != '\0'; ++count)
        ;

    // returning the character count of the string
    return count + 1;
}
struct ast *fill_node(struct ast *arbre)
{
    struct ast *ret = malloc(sizeof(struct ast));
    ret->nb_child = 0;
    ret->capacity = 1;
    ret->child = malloc(sizeof(struct ast *));
    ret->child[0] = NULL;
    if (arbre == NULL)
    {
        return ret;
    }
    if (arbre->capacity - 1 <= arbre->nb_child)
    {
        arbre->capacity *= 2;
        arbre->child =
            realloc(arbre->child, sizeof(struct ast *) * arbre->capacity);
    }
    arbre->child[arbre->nb_child] = ret;

    arbre->nb_child += 1;
    arbre->child[arbre->nb_child] = NULL;

    return ret;
}
int add_child(struct ast *arbre, struct ast *child)
{
    if (arbre == NULL || child == NULL)
    {
        return -1;
    }
    if (arbre->capacity - 1 <= arbre->nb_child)
    {
        arbre->capacity *= 2;
        arbre->child =
            realloc(arbre->child, sizeof(struct ast *) * arbre->capacity);
    }
    arbre->child[arbre->nb_child] = child;
    arbre->nb_child += 1;
    arbre->child[arbre->nb_child] = NULL;
    return 1;
}

int exist_in_if_grammar(char *elt)
{
    if (elt == NULL)
    {
        return -1;
    }
    const char *gram[20];

    gram[0] = ";";
    gram[1] = "\n";
    gram[2] = "if";
    gram[3] = "then";
    gram[4] = "else";
    gram[5] = "fi";
    gram[6] = "|";
    gram[7] = "do";
    gram[8] = "done";
    gram[9] = "while";
    gram[10] = "until";
    gram[11] = "||";
    gram[12] = "&&";
    gram[13] = "for";
    gram[14] = "{";
    gram[15] = "}";

    gram[16] = NULL;
    int j = 0;
    while (gram[j] != NULL)
    {
        if (strcmp(gram[j], elt) == 0)
        {
            return 1;
        }
        j += 1;
    }
    return 0;
}

int exist_in(char *elt)
{
    if (elt == NULL)
    {
        return -1;
    }
    const char *gram[10];

    gram[0] = ";";
    gram[1] = "\n";
    gram[2] = "||";
    gram[3] = "&&";
    gram[4] = "|";
    gram[5] = ")";
    gram[6] = "(";

    gram[7] = NULL;
    int j = 0;
    while (gram[j] != NULL)
    {
        if (strcmp(gram[j], elt) == 0)
        {
            return 1;
        }
        j += 1;
    }
    return 0;
}
void free_ast(struct ast *arbre)
{
    while (arbre->nb_child != 0)
    {
        free_ast(arbre->child[arbre->nb_child - 1]);
        arbre->nb_child--;
    }
    free(arbre->child);
    free(arbre->node);
    free(arbre);
}
