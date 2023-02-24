#include "include.h"

void my_exit(char *command, int *retour)
{
    while (*command != 0 && *command == ' ')
    {
        command++;
    }
    if (*command != 0)
    {
        *retour = atoi(command);
    }
}
