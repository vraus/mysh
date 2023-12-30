#ifndef TOKEN_HANDLER_H
#define TOKEN_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

// colors for the prints
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"

#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(RED msg RESET);    \
        exit(status);             \
    } while (0)

#define handle_error_noexit(msg) \
    do                           \
    {                            \
        perror(RED msg RESET);   \
    } while (0)

/**
 * @brief Retrieves the command entered by the user.
 *
 * @param str Pointer to a character array to store the command.
 */
void getcommand(char *str);

/**
 * @brief Checks if a character is a space or tab
 *
 * @param ch the character to check
 * @return true if the character is a space or tab, false otherwise
 */
bool is_space_or_tab(char ch);

/**
 * @brief Tokenizes a command string into an array of arguments based on space and tab characters.
 *
 * This function fragments a command string into multiple strings based on spaces and tabs.
 * It populates the 'args' array with pointers to individual arguments.
 *
 * @param str `char *` The command string to tokenize.
 * @param args `char **` Array to store pointers to individual arguments.
 */
void tokenize_space(char *str, char *args[]);

/**
 * @brief Tokenizes a command string into an array of commands.
 *
 * This function tokenizes a command string based on special characters ('&', '|', ';').
 * It modifies the input string and populates the 'commands' array with pointers to individual commands.
 * The 'command_count' variable is updated to reflect the number of commands found.
 *
 * @param str `char *` The command string to tokenize.
 * @param commands `char **` Array to store pointers to individual commands.
 * @param command_count `int *` Variable to store the number of commands found.
 */
void tokenize(char *str, char *commands[], int *command_count);

/**
 * @brief Modifies the bitmask to indicate the options entered by the user.
 * @param args `char **` Array containing command words.
 * @param mask `int *` Bitmask indicating options for the myls command.
 */
void hasOption(char **args, int *mask);

/**
 * @brief Executes the "myls" command if entered by the user.
 * @param mask `int *` Bitmask indicating options for the myls command.
 */
void is_myls(int *mask, char *myls_realpath);

/**
 * @brief Called by `execute_command` when the user enters `myps` in the terminal.
 * `myps` is an external program similar to the `ps aux` command in bash.
 */
void is_myps(char *myps_realpath);

#endif