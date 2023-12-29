#ifndef WILDCARD_HANDLER_H
#define WILDCARD_HANDLER_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <fnmatch.h>
#include <dirent.h>

/**
 * @brief Function called when wildcard used in a command.
 * @param pattern The wildcard's pattern used with the command
 * @param command The command which the user used
 * @param args Possible args used with the command
 */
void execute_command_with_wildcards(char *pattern, char *command, char **args);

#endif