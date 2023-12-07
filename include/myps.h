#ifndef MYPS_H
#define MYPS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define DEFAULT "\033[1;0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"

#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(DEFAULT msg);      \
        exit(status);             \
    } while (0)

void print_process_info(int pid);

int main(int argc, char *argv[]);

#endif