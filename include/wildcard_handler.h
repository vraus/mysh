#ifndef WILDCARD_HANDLER_H
#define WILDCARD_HANDLER_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <fnmatch.h>
#include <dirent.h>

void execute_command_with_wildcards(char *pattern, char *command, char **args);

#endif