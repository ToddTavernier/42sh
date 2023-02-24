#include "lexer3.h"

char *token_is_unset(char *str, struct variables *vars)
{
    str++;
    while (*str != '\n' && *str != ';')
    {
        int i = 0;
        while (str[i] != ' ' && str[i] != '\n' && str[i] != ';')
            i++;
        char *a = calloc(i + 1, sizeof(char));
        strncpy(a, str, i);
        if (str[i] == ' ')
            str++;
        change_var(a, vars);
        str += i;
        free(a);
    }
    return str;
}

int size_str(char *str)
{
    if (str == NULL)
    {
        return -1;
    }
    int count;
    for (count = 0; str[count] != '\0'; ++count)
        ;
    return count + 1;
}

int islimit(char c, size_t single_quote, size_t double_quote)
{
    if ((single_quote == 1 && c == 39) || c == '\0')
        return 0;
    else if (double_quote == 1 && (c == '"' || c == '$' || c == ')'))
        return 0;
    else if (double_quote == 0 && single_quote == 0
             && (isspace(c) || c == ';' || c == 39 || c == ')' || c == '('))
        return 0;
    return 1;
}

char *add_token(char **token, size_t *token_index, size_t taille, char *str)
{
    token[*token_index] = calloc(taille + 1, sizeof(char));
    if (taille == 1)
        token[*token_index][0] = *str;
    else
        strncpy(token[*token_index], str, taille);
    *token_index += 1;
    if (taille == 0)
    {
        str++;
        return str;
    }
    str += taille;
    return str;
}

char *str_EOL(size_t *var_set, char **token, size_t *token_index, char *str)
{
    if (var_set == NULL)
    {}
    var_set = 0;
    if (*token_index >= 1)
    {
        str = add_token(token, token_index, 1, str);
        return str;
    }
    else
    {
        str++;
        return str;
    }
}

void realloc_token(char **token, size_t *token_index, size_t taille, char *str)
{
    if (taille <= 0)
        return;

    token[*token_index - 1] =
        realloc(token[*token_index - 1],
                size_str(token[*token_index - 1]) * sizeof(char) + taille);
    strncat(token[*token_index - 1], str, taille);
}

char *help_lex_dollar(char *str, char **token, struct var_lexer *var_lex)
{
    if (*str == '(' && *(str + 1) != '(')
    {
        str++;
        add_token(token, &*var_lex->token_index, 1, "~");
        *var_lex->var_set = 1;
        *var_lex->nb_char_open = 1;

        *var_lex->my_continue = 1;
        return str;
    }
    return str;
}

char *lex_dollar(char *str, char **token, struct var_lexer *var_lex,
                 struct variables *vars)
{
    *var_lex->my_continue = 1;
    if (str[*var_lex->str_index] == '$' && *var_lex->single_quote == 0)
    {
        str++;
        str = help_lex_dollar(str, token, var_lex);
        char *res =
            lex_variable(str, var_lex->nb_char_open, var_lex->argv, vars);
        size_t len_res = 0;
        if (res)
        {
            len_res = strlen(res);
            if (strcmp(res, "count_bad_cara") == 0)
            {
                *var_lex->my_continue = -1;
                return str;
            }
        }
        if (*var_lex->token_index == 0
            || token[*var_lex->token_index - 1][0] == '"'
            || *var_lex->double_quote == 0)
            add_token(token, &*var_lex->token_index, len_res, res);
        else
            realloc_token(token, &*var_lex->token_index, len_res, res);
        while (*str != ' ' && *str != '\0' && *str != '$' && *str != '"'
               && *str != '}' && *str != ')' && *str != '\n')
            str++;
        if (*var_lex->nb_char_open == 2)
        {
            free(res);
        }
        if (!*str && *var_lex->nb_char_open != 0)
        {
            *var_lex->my_continue -= 1;
            return str;
        }
        while (str && (*str == ')' || *str == '}')
               && *var_lex->nb_char_open > 0)
        {
            str++;
            *var_lex->nb_char_open -= 1;
        }
        if (*var_lex->nb_char_open > 0)
        {
            *var_lex->my_continue -= 1;
            return str;
        }
        *var_lex->var_set = 1;
        *var_lex->my_continue = 0;
        return str;
    }
    return str;
}

void free_var_lexer(struct var_lexer *var_lex)
{
    free(var_lex->double_quote);
    free(var_lex->single_quote);
    free(var_lex->escape_char);
    free(var_lex->my_continue);
    free(var_lex->nb_char_open);
    free(var_lex->str_index);
    free(var_lex->token_index);
    free(var_lex->var_set);
    free(var_lex);
}

int retour_error(char *classique_token)
{
    free(classique_token);
    return 127;
}
