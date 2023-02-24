#include "lexer.h"

struct var_lexer *initiate(char *argv[])
{
    struct var_lexer *var_lex = malloc(sizeof(struct var_lexer));

    var_lex->str_index = malloc(sizeof(int));
    var_lex->token_index = malloc(sizeof(size_t));
    var_lex->single_quote = malloc(sizeof(size_t));
    var_lex->double_quote = malloc(sizeof(size_t));
    var_lex->var_set = malloc(sizeof(size_t));
    var_lex->escape_char = malloc(sizeof(size_t));
    var_lex->nb_char_open = malloc(sizeof(int));
    var_lex->my_continue = malloc(sizeof(int));

    var_lex->argv = argv;
    *var_lex->token_index = 0;
    *var_lex->str_index = 0;
    *var_lex->single_quote = 0;
    *var_lex->double_quote = 0;
    *var_lex->escape_char = 0;
    *var_lex->var_set = 0;
    *var_lex->nb_char_open = 0;

    return var_lex;
}

char *lex_EOL(char *str, char **token, struct var_lexer *var_lex)
{
    *var_lex->my_continue = 1;
    if (str[*var_lex->str_index] == '\n')
    {
        str = str_EOL(&*var_lex->var_set, token, &*var_lex->token_index, str);
        *var_lex->my_continue = 0;
        return str;
    }
    if (str[*var_lex->str_index] == ';' || str[*var_lex->str_index] == '('
        || str[*var_lex->str_index] == ')' || str[*var_lex->str_index] == '}'
        || str[*var_lex->str_index] == '{')
    {
        if (*var_lex->token_index == 0)
        {
            str++;
            *var_lex->my_continue = 0;
            return str;
        }
        if (str[*var_lex->str_index] != ';' && *var_lex->var_set == 1)
        {
            if (*var_lex->nb_char_open == 0)
            {
                realloc_token(token, &*var_lex->token_index, 1, str);
                str++;
                *var_lex->my_continue = 0;
                return str;
            }
            *var_lex->var_set = 0;
            str++;
            *var_lex->my_continue = 0;
            return str;
        }
        else if (str[*var_lex->str_index] == ';')
            *var_lex->var_set = 0;
        str = add_token(token, &*var_lex->token_index, 1, str);
        *var_lex->my_continue = 0;
        return str;
    }
    return str;
}

char *lexing(char *str, char **token, struct var_lexer *var_lex,
             struct variables *vars)
{
    if (token == NULL)
    {
        *var_lex->my_continue = 1;
        return str;
    }
    *var_lex->str_index = 0;
    while (
        (isspace(str[*var_lex->str_index]) && str[*var_lex->str_index] != '\n')
        && *var_lex->single_quote == 0 && *var_lex->double_quote == 0)
        str++;
    if (*str == '\0')
    {
        *var_lex->my_continue = 0;
        return str;
    }
    *var_lex->my_continue = 1;
    str = lex_dollar(str, token, var_lex, vars);
    if (*var_lex->my_continue == -1)
    {
        return str;
    }
    if (*var_lex->my_continue == 0)
    {
        return str;
    }

    if (str[*var_lex->str_index] == '#' && *var_lex->single_quote == 0
        && *var_lex->double_quote == 0)
    {
        *var_lex->var_set = 0;
        while (str[*var_lex->str_index] != '\n'
               && str[*var_lex->str_index] != '\0')
            str++;

        *var_lex->my_continue = 0;
        return str;
    }
    return str;
}

char *help_lexer(char *str, char **token, struct var_lexer *var_lex,
                 struct variables *vars)
{
    str = lexing(str, token, var_lex, vars);
    if (*var_lex->my_continue == -1)
    {
        return str;
    }
    if (*var_lex->my_continue == 0)
    {
        return str;
    }
    str = lex_EOL(str, token, var_lex);
    if (*var_lex->my_continue == -1)
        return str;

    if (*var_lex->my_continue == 0)
        return str;

    if (str[0] == '\'' && *var_lex->double_quote == 0) // cas d'une guillemet
    {
        if (*var_lex->token_index > 0
            && token[*var_lex->token_index - 1][0] == '"')
            add_token(token, &*var_lex->token_index, 0, str);
        add_token(token, &*var_lex->token_index, 1, "\"");
        str++;
        *var_lex->single_quote = (*var_lex->single_quote + 1) % 2;
        *var_lex->var_set = 0;

        *var_lex->my_continue = 0;
        return str;
    }
    if (str[0] == '"' && *var_lex->single_quote == 0)
    {
        if (*var_lex->token_index > 0
            && token[*var_lex->token_index - 1][0] == '"')
            add_token(token, &*var_lex->token_index, 0, str);
        str = add_token(token, &*var_lex->token_index, 1, str);
        *var_lex->double_quote = (*var_lex->double_quote + 1) % 2;
        *var_lex->escape_char = (*var_lex->escape_char + 1) % 2;
        *var_lex->var_set = 0;

        *var_lex->my_continue = 0;
        return str;
    }
    return str;
}
struct hlp_var
{
    char *str;
    struct var_lexer *var_lex;
    struct variables *var;
    struct variables *vars;
    char *classique_token;
};

char *help_var(struct hlp_var *save_var)
{
    save_var->var->name =
        calloc(*save_var->var_lex->str_index + 1, sizeof(char));
    strncpy(save_var->var->name, save_var->classique_token,
            *save_var->var_lex->str_index);
    save_var->str++;
    int i = 0;
    while (islimit(save_var->str[i], *save_var->var_lex->single_quote,
                   *save_var->var_lex->double_quote))
    {
        i++;
    }
    save_var->var->value = calloc(i + 1, sizeof(char));
    strncpy(save_var->var->value, save_var->str, i);

    save_var->str += i;
    // free(save_var->classique_token);
    return save_var->str;
}

char *lexer_close(char *str, char **token, struct var_lexer *var_lex,
                  struct hlp_var *save_vars)
{
    *var_lex->my_continue = 1;
    if (strcmp(save_vars->classique_token, "unset") == 0)
    {
        str = token_is_unset(str, save_vars->vars);
        *var_lex->my_continue = 0;
        return str;
    }
    if (*var_lex->double_quote == 1 && *var_lex->var_set == 1)
        realloc_token(token, &*var_lex->token_index, *var_lex->str_index,
                      save_vars->classique_token);
    else
        add_token(token, &*var_lex->token_index, *var_lex->str_index,
                  save_vars->classique_token);
    return str;
}

int lexer_return(char **token, struct var_lexer *var_lex,
                 struct variables *vars)
{
    int ret = *var_lex->token_index;
    token[*var_lex->token_index] = NULL;
    free_vars(vars);
    if (*var_lex->double_quote == 1 || *var_lex->single_quote == 1)
        ret = -2;
    if (token[0] == NULL && *var_lex->token_index != 0)
        ret = -1;
    free_var_lexer(var_lex);
    return ret;
}
char *forty_max(struct hlp_var *save_var, char *str, struct var_lexer *var_lex,
                struct variables *vars)
{
    struct variables *var = malloc(sizeof(struct variables)); // 30
    save_var->var = var;
    save_var->str = str;
    save_var->var_lex = var_lex;
    str = help_var(save_var); // 35
    save_var->var->next = vars;
    vars = save_var->var;
    return str;
}

struct hlp_var initiate_save_var(struct variables *vars)
{
    struct hlp_var save_var = {
        .var = NULL,
        .vars = vars,
    };
    return save_var;
}

// lexer partant d'une string et d'une liste de token vide
size_t lexer(char *str, char **token, char *argv[])
{
    struct var_lexer *var_lex = initiate(argv);
    struct variables *vars = NULL;
start:
    while (str[0] != '\0')
    {
        str = help_lexer(str, token, var_lex, vars); // 5
        if (*var_lex->my_continue == -1)
            return -1;

        if (*var_lex->my_continue == 0)
            continue;

        char *classique_token = calloc(strlen(str), sizeof(char)); // 10
        int escape = 0;
        struct hlp_var save_var = initiate_save_var(vars);
        // struct hlp_var save_var = {
        //     .var = NULL,
        //     .vars = vars,
        // };
        // save_var.classique_token = classique_token; // 15

        while (islimit(*str, *var_lex->single_quote,
                       *var_lex->double_quote)) // cas de lettre
        {
            if (*var_lex->double_quote && *str == '\\')
            {
                escape_char_str(str, &*var_lex->str_index, &escape,
                                classique_token);
                save_var.classique_token = classique_token;
                str += 2;
                continue;
            }
            else if (*str == '=' && !*var_lex->double_quote
                     && !*var_lex->single_quote) // var vue
            {
                if (str[1] == ' ' || str[-1] == ' ') // 28
                    return retour_error(classique_token);
                save_var.classique_token = classique_token;
                str = forty_max(&save_var, str, var_lex, vars);
                save_var.var->next = vars;
                vars = save_var.var; // 30
                free(classique_token);
                goto start;
            }
            classique_token[*var_lex->str_index] = *str; // 40
            *var_lex->str_index += 1;
            str++;
        }
        save_var.classique_token = classique_token;
        str = lexer_close(str, token, var_lex, &save_var);
        free(classique_token);
        if (*var_lex->my_continue == 0)
            continue; // 45
    }
    return lexer_return(token, var_lex, vars);
}
