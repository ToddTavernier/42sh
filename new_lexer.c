#include "new_lexer.h"

char *switch_UID(void)
{
    char *buffer = calloc(128, sizeof(char));
    uid_t user_uid = getuid();
    unsigned int uid = (unsigned int)user_uid;
    sprintf(buffer, "%u", uid);
    return buffer;
}

char *switch_at(int count_arg, char **argv)
{
    int i = 0;
    int arg_len = 0;
    while (i < count_arg)
    {
        arg_len += strlen(argv[i]);
        i++;
    }
    char *buffer = calloc(arg_len + i + 1, sizeof(char));
    for (i = 0; i < count_arg - 1; i++)
    {
        strcat(buffer, argv[i]);
        strcat(buffer, "\n");
    }
    strcat(buffer, argv[i]);
    return buffer;
}

char *switch_star(int count_arg, char **argv)
{
    int i = 0;
    int arg_len = 0;
    while (i < count_arg)
    {
        arg_len += strlen(argv[i]);
        i++;
    }
    char *buffer = calloc(arg_len + i + 1, sizeof(char));
    for (i = 0; i < count_arg - 1; i++)
    {
        strcat(buffer, argv[i]);
        strcat(buffer, " ");
    }
    strcat(buffer, argv[i]);
    return buffer;
}

char *switch_variable(char *a, char **argv)
{
    int count_arg = 0;
    while (argv && argv[count_arg])
        count_arg++;
    if (strcmp(a, "PWD") == 0)
    {
        char *buffer = calloc(128, sizeof(char));
        getcwd(buffer, 128);
        return buffer;
    }
    if ('0' <= *a && *a <= '9')
    {
        int var_value = atoi(a);
        char *buffer = calloc(strlen(argv[var_value]) + 1, sizeof(char));
        strncpy(buffer, argv[var_value], strlen(argv[var_value]));
        return buffer;
    }
    if (strcmp(a, "UID") == 0)
    {
        return switch_UID();
    }
    if (strcmp(a, "RANDOM") == 0)
    {
        char *buffer = calloc(17, sizeof(char));
        srand(time(NULL));
        unsigned int random = rand() % 32767;
        sprintf(buffer, "%u", random);
        return buffer;
    }
    if (strcmp(a, "#") == 0)
    {
        char *buffer = calloc(128, sizeof(char));
        sprintf(buffer, "%u", count_arg);
        return buffer;
    }
    if (strcmp(a, "@") == 0)
    {
        return switch_at(count_arg, argv);
    }
    if (strcmp(a, "*") == 0)
    {
        return switch_star(count_arg, argv);
    }
    if (strcmp(a, "$") == 0)
    {
        char *buffer = calloc(128, sizeof(char));
        pid_t user_pid = getpid();
        unsigned int pid = (unsigned int)user_pid;
        sprintf(buffer, "%u", pid);
        return buffer;
    }
    return NULL;
}

void change_var(char *unset_var, struct variables *vars)
{
    struct variables *copy = vars;
    while (copy)
    {
        if (strcmp(copy->name, unset_var) == 0)
        {
            copy->name[0] = '\0';
            break;
        }
        copy = copy->next;
    }
}

char *eval_inter(char *a)
{
    int err = 0;
    int res_evalxpr = evalxpr(a, &err);
    free(a);
    if (err == 3)
        return "error : division by 0";
    char *buffer = calloc(128, sizeof(char));
    sprintf(buffer, "%u", res_evalxpr);
    buffer = realloc(buffer, strlen(buffer) + 1);
    return buffer;
}

char char_esc_b(int *escape)
{
    *escape += 1;
    return '\b';
}

char check_char_esc(char c, int *escape)
{
    switch (c)
    {
    case 'a':
        c = '\a';
        *escape += 1;
        break;
    case 'b':
        c = char_esc_b(escape);
        break;
    case 'c':
        return '\0';
    case 'e':
        c = '\x1B';
        *escape += 1;
        break;
    case 'f':
        *escape += 1;
        c = '\f';
        break;
    case 'n':
        *escape += 1;
        c = '\n';
        break;
    case 'r':
        *escape += 1;
        c = '\r';
        break;
    case 't':
        *escape += 1;
        c = '\t';
        break;
    case 'v':
        *escape += 1;
        c = '\v';
        break;
    case '\\': {
        *escape += 1;
        c = '\\';
    }
    default: {
        break;
    }
    }
    return c;
}

char *escape_char_str(char *str, int *str_index, int *escape,
                      char *classique_token)
{
    unsigned char c;
    int prev_esc = *escape;
    c = check_char_esc(str[1], escape);

    if (*escape != prev_esc)
    {
        classique_token[*str_index] = c;
        *str_index += 1;
    }
    else
    {
        classique_token[*str_index] = c;
        *str_index += 1;
    }
    return classique_token;
}

char *lex_variable(char *str, int *open_char, char *argv[],
                   struct variables *vars)
{
    size_t i = 0;
    int count_par = 0;
    int count_acol = 0;
    for (; *str == '{' || *str == '('; str++)
    {
        if (*str == '(')
            count_par++;
        if (*str == '{')
            count_acol++;
    }
    if (count_acol > 1 || (count_acol >= 1 && count_par >= 1))
        return "count_bad_cara";
    *open_char = (count_par > count_acol) ? count_par : count_acol;
    while (str[i] != ' ' && str[i] != '\0' && str[i] != '$' && str[i] != '"'
           && str[i] != '}' && str[i] != ')' && str[i] != '\n')
        i++;
    char *a = calloc(i + 1, sizeof(char));
    i = (i == 0) ? 1 : i;
    strncpy(a, str, i);
    if (count_par == 2)
    {
        return eval_inter(a);
    }
    char *value = "";
    struct variables *save = vars;
    while (vars)
    {
        if (strcmp(a, vars->name) == 0)
        {
            value = vars->value;
            break;
        }
        vars = vars->next;
    }
    vars = save;
    if (strcmp(value, "") == 0)
    {
        char *res = switch_variable(a, argv);
        free(a);
        if (res != NULL)
            return res;
        else
        {
            return NULL;
        }
    }
    else
    {
        free(a);
        return value;
    }
}
