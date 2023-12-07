#ifndef MYPS_H
#define MYPS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(msg);              \
        exit(status);             \
    } while (0)

int main(int argc, char *argv[]);

#endif