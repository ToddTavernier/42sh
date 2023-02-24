#include "redirection.h"

int esperluete(struct ast *arbre, int left_fd, int dest, int fd_dup)
{
    if (dup2(dest, left_fd) == -1)
        return 1;
    execute_command(arbre->child[0]);
    // Ensure the message is printed before dup2 is performed
    fflush(stdout);
    dup2(fd_dup, dest);
    close(fd_dup);
    return 0;
}

int switch1(struct ast *arbre, int operation, int index, int left_fd)
{
    arbre->node += index; // deplacement de la node jusqu'au nom du fichier
    int fd_dup = dup(left_fd); //  > (1)   >> (2)  >& (3)  >| (4)
    if (fd_dup == -1)
        return 1;
    int res = 0;
    int right_fd = 0;
    switch (operation)
    {
    case 1:
        res = write_over(arbre, left_fd, fd_dup, "w");
        arbre->node -= index;
        return res;
        break;
    case 2:
        res = write_over(arbre, left_fd, fd_dup, "a");
        arbre->node -= index;
        return res;
        break;
    case 3:
        right_fd += atoi(arbre->node);
        res = esperluete(arbre, left_fd, right_fd, fd_dup);
        arbre->node -= index;
        return res;
        break;
    case 4:
        res = write_over(arbre, left_fd, fd_dup, "w");
        arbre->node -= index;
        return res;
        break;

    default:
        break;
    }
    return 0;
}

int to_left(struct ast *arbre)
{
    char *content = arbre->node;
    int left_fd = 1;
    int operation = 0;
    int index = 0;
    if (isdigit(content[0])) // test si on a '1' &>
    {
        index = 1;
        left_fd = content[0] - 48;
    }
    if (content[index + 1] && content[index] == '>')
    {
        switch (content[index + 1])
        {
        case '>':
            operation = 2;
            index++;
            break;
        case '&':
            operation = 3;
            index++;
            break;
        case '|':
            operation = 4;
            index++;
            break;
        default:
            operation = 1;
            break;
        }
        index++; // cas ou on a une redirection + longue
    }
    return switch1(arbre, operation, index, left_fd);
    /*arbre->node -= index;
    return 0;*/
}

int switch2(struct ast *arbre, int operation, int index, int right_fd)
{
    arbre->node += index;
    int left_fd = 0;
    int fd_dup = dup(right_fd);
    if (fd_dup == -1)
        return 1;
    int res = 0;
    switch (operation) //  < (5)   <& (6)  <> (7)
    {
    case 5:
        res = write_over(arbre, right_fd, 0, "ir");
        arbre->node -= index;
        return res;
        break;
    case 6:
        left_fd += atoi(arbre->node);
        res = esperluete(arbre, left_fd, right_fd, fd_dup);
        arbre->node -= index;
        return res;
        break;
    case 7:
        res = write_over(arbre, right_fd, 0, "iw+");
        arbre->node -= index;
        return res;
        break;
    default:
        break;
    }
    return 0;
}

int to_right(struct ast *arbre)
{
    char *content = arbre->node;
    int right_fd = 0;
    int operation = 0;
    int index = 0;
    if (isdigit(content[0]))
    {
        index = 1;
        right_fd = content[0] - 48;
    }
    if (content[index + 1] && content[index] == '<')
    {
        if (content[index + 1] == '&')
        {
            if (!content[index - 1])
                right_fd = 1;
            index++;
            operation = 6;
        }
        else if (content[index] == '>')
        {
            index++;
            operation = 7;
        }
        index++;
    }
    else
        operation = 5;
    return switch2(arbre, operation, index, right_fd);
}

int write_over(struct ast *arbre, int fd, int fd_dup, char *autorisation)
{
    FILE *fichier = NULL;
    int inverted = 0;
    if (*autorisation == 'i')
    {
        inverted = 1;
        autorisation++;
    }
    fichier = fopen(arbre->node,
                    autorisation); // ecrase le fichier si existe le cree sinon,
                                   // le pointeur au debut du fichier
    int fichier_fd = fichier->_fileno;
    if (inverted)
        fd_dup = dup(fichier_fd);
    if (fd_dup == -1)
        return 1;
    if (fichier == NULL)
    {
        return 1;
    }
    if (inverted == 1 && dup2(fd, fichier_fd) == -1)
        return 1;
    else if (dup2(fichier_fd, fd) == -1)
        return 1;
    if (autorisation[0] == 'r')
    {
        dup2(fd_dup, fd);
        close(fd_dup);
        fclose(fichier);
        execute_command(arbre->child[0]);
    }
    else
    {
        execute_command(arbre->child[0]);
        fflush(stdout);
        dup2(fd_dup, fd);
        close(fd_dup);
        fclose(fichier);
    }
    return 0;
}
