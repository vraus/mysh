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
#define CYAN "\033[1;36m"

#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(DEFAULT msg);      \
        exit(status);             \
    } while (0)

/**
 * @brief Prints information about a process based on its process ID (PID).
 *
 * This function reads information from the /proc/<pid>/status and /proc/<pid>/cmdline files
 * to retrieve details such as UID, PID, PPID, STAT, NAME, and COMMAND.
 *
 * @param pid `int` The process ID for which information is to be printed.
 */
void print_process_info(int pid);

/**
 * @brief Entry point for the myps program. Lists information about all processes on the system.
 *
 * This function opens the /proc directory, iterates through its entries, and prints information
 * about each process using the print_process_info function.
 *
 * @param argc `int` Number of command-line arguments.
 * @param argv `char *[]` Array of command-line argument strings.
 * @return `int` Exit status of the program.
 */
int main(int argc, char *argv[]);

#endif
