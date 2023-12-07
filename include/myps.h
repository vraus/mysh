#ifndef MYPS_H
#define MYPS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(msg);              \
        exit(status);             \
    } while (0)

void print_process_info(int pid);

int main(int argc, char *argv[]);

#endif