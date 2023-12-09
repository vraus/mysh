#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>

// colors for the prints
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"

#define COMMAND_LENGTH 2048

#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(RESET msg);        \
        exit(status);             \
    } while (0)

#define handle_error_noexit(msg) \
    do                           \
    {                            \
        perror(RESET msg);       \
    } while (0)

/**
 * @brief Retrieves the command entered by the user.
 *
 * @param str Pointer to a character array to store the command.
 */
void getcommand(char *str)
{
    int i = 0, ch;

    // Read characters from standard input until newline or EOF
    while ((ch = getchar()) != EOF)
    {
        if (ch == '\n')
            break; // Exit the loop when a newline character is encountered

        str[i++] = (char)ch; // Store the character in the command string
    }

    // Check for errors in reading from stdin
    if (ferror(stdin))
    {
        perror("Error reading input");
        exit(EXIT_FAILURE);
    }

    str[i] = '\0'; // Null-terminate the command string
}

/**
 * @brief Checks if a character is a space or tab
 *
 * @param ch the character to check
 * @return true if the character is a space or tab, false otherwise
 */
bool is_space_or_tab(char ch) { return ch == ' ' || ch == '\t'; }

/**
 * @brief Tokenizes a command string into an array of arguments based on space and tab characters.
 *
 * This function fragments a command string into multiple strings based on spaces and tabs.
 * It populates the 'args' array with pointers to individual arguments.
 *
 * @param str `char *` The command string to tokenize.
 * @param args `char **` Array to store pointers to individual arguments.
 */
void tokenize_space(char *str, char *args[])
{
    int len = strlen(str), j, i = 0;

    for (j = 0; j < len; ++j)
    {
        // Skip consecutive spaces or tabs
        while (is_space_or_tab(str[j]))
            ++j;

        if (j == len)
            break;

        // Point to the beginning of each argument
        args[i] = &str[j];

        // Find the end of the current argument
        while ((j < len) && (!is_space_or_tab(str[j])))
        {
            if (str[j] == '=')
            {
                // If '=' is encountered null-terminate the current argument
                str[j] = '\0';
                args[++i] = "=";
                args[++i] = &str[++j]; // Continue parsing from the position agter '='
                continue;
            }
            ++j;
        }

        // Null-terminate the argument
        str[j] = '\0';
        ++i;
    }

    args[i] = NULL; // Null-terminate the arguments array
}

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
void tokenize(char *str, char *commands[], int *command_count)
{
    int len = strlen(str), i = 0, j;
    for (j = 0; j < len; j++)
    {
        // Handle cases where '&' or '|' follows a space
        if (j - 1 > 0 && (str[j] == '&' || str[j] == '|'))
        {
            if (str[j - 1] == ' ')
            {
                str[j - 1] = '\0';
            }
        }

        // Handle cases where ';' or a single '&' or '|' is encountered
        if (j - 1 > 0 && (str[j] == ';' || (str[j - 1] == '&' && str[j] != '&') || (str[j - 1] == '|' && str[j] != '|')))
            str[j] = '\0';
        else if (j == 0 || str[j - 1] == '\0')
        {
            commands[i] = &str[j]; // Point to the beginning of each argument
            i++;
        }
    }

    *command_count = i;
    commands[i] = NULL; // Null-terminate the commands array
}

/**
 * @brief Modifies the bitmask to indicate the options entered by the user.
 * @param args `char **` Array containing command words.
 * @param mask `int *` Bitmask indicating options for the myls command.
 */
void hasOption(char **args, int *mask)
{
    int opt, argc = 0;

    // Count the number of arguments
    for (int i = 0; args[i] != NULL; i++)
        argc++;

    // Parse options using getopt
    while ((opt = getopt(argc, args, "Ra")) != -1)
    {
        switch (opt)
        {
        case 'a':
            // Set the '-a' option in the bitmask
            *mask |= (1 << 0);
            break;
        case 'R':
            // Set the '-R' option in the bitmask
            *mask |= (1 << 1);
            break;

        default:
            // Print an error message for invalid options
            printf(RED "Invalid option.\n" RESET);
            break;
        }
    }
}

/**
 * @brief Executes the "myls" command if entered by the user.
 * @param mask `int *` Bitmask indicating options for the myls command.
 */
void is_myls(int *mask)
{
    char *myls_args[4];
    myls_args[0] = "./external/myls";

    int arg_count = 1;

    // Check if the '-a' option is set in the mask
    if (*mask & (1 << 0))
        myls_args[arg_count++] = "-a";

    // Check if the '-R' option is set in the mask
    if (*mask & (1 << 1))
        myls_args[arg_count++] = "-R";

    myls_args[arg_count] = NULL;

    // Execute the external "myls" program with specified options
    if (execvp(myls_args[0], myls_args) == -1)
        handle_error("execvp", -1);
}

/**
 * @brief Called by `execute_command` when the user enters `myps` in the terminal.
 * `myps` is an external program similar to the `ps aux` command in bash.
 */
void is_myps()
{
    char *args[2];
    args[0] = "./external/myps";
    args[1] = NULL;

    // Execute external "myps" programm
    if (execvp(args[0], args) == -1)
        handle_error("execl (myps)", -1);
}

/**
 * @brief Executes commands using execvp.
 * @param mask `int *` Bitmask indicating options for the myls command.
 * @param args `char **` Array containing command arguments.
 */
void execute_command(int *mask, char *args[])
{
    if (strcmp(args[0], "myls") == 0) // Check if the command is "myls"
    {
        hasOption(args, mask); // Parse options for the "myls" command
        is_myls(mask);         // Execute the "myls" command with specified options
    }
    else if (strcmp(args[0], "myps") == 0) // Check if the command is "myps"
        is_myps();                         // Execute the "myps" command
    else if (strcmp(args[0], "set") == 0)
        printf("args[0]:%s, args[1]:%s, args[2]:%s, args[3]:%s\n", args[0], args[1], args[2], args[3]);
    else if (execvp(args[0], args) == -1) // If it's neither "myls" nor "myps", execute the command using execvp
        handle_error("Command execution error", -1);
}

int main()
{
    char input[COMMAND_LENGTH], *command[COMMAND_LENGTH], *args[20];
    int wstatus, command_count = 0;
    int mask = 0x000;

    pid_t bg_pid; // pid for background process
    for (;;)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));

        mask = 0x000; // Reset the mask

        // Detecting the mysh part of the working dir
        char *project = strstr(cwd, "/mysh");
        if (project != NULL)
            printf(BLUE " %s > " RESET, project);
        else
            printf(GREEN " > ");

        // Read the user's command
        getcommand(input);
        if (strcmp(input, "exit") == 0)
        {
            break; // Exit the tinyshell
        }

        // Tokenizes the command into individual commands
        tokenize(input, command, &command_count);

        // Array to store child process PIDs
        pid_t child_pids[command_count];
        // Loop through each command of the input
        for (int i = 0; i < command_count; i++)
        {
            // Skip logical operators
            if (strcmp(command[i], "&&") == 0 || strcmp(command[i], "||") == 0 || strcmp(command[i], "&") == 0)
                continue;
            else
            {
                // Check if the command is intented to run in the background
                if (i + 1 < command_count && strcmp(command[i + 1], "&") == 0)
                {
                    // Fork the new process for the bg command
                    if ((bg_pid = fork()) == -1)
                        handle_error_noexit("fork (bg_pid)");

                    // Child process: executes the bg command
                    if (bg_pid == 0)
                    {
                        tokenize_space(command[i], args);
                        execute_command(&mask, args);
                        exit(EXIT_SUCCESS); // Make sure the bg_pid exits
                    }
                }
                else
                {
                    // Fork a new process for the foreground command
                    child_pids[i] = fork();
                    if (child_pids[i] == -1)
                        handle_error("fork (childs_pid)", -1);

                    // Child process: Executes forground command
                    if (child_pids[i] == 0)
                    {
                        tokenize_space(command[i], args);
                        execute_command(&mask, args);
                    }
                    else
                    {
                        // Parent process: Wait for the child to finish and check for logical operators
                        if (waitpid(child_pids[i], &wstatus, 0) < 0)
                            handle_error("Error waitpid", -1);

                        // Check if the command failed (non-zero exit status)
                        if (i + 1 < command_count && strcmp(command[i + 1], "&&") == 0)
                        {
                            if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
                            {
                                printf("Command failed. Skipping subsequent commands.\n");
                                break; // Stop executing subsequent commands
                            }
                        }
                        else if (i + 1 < command_count && strcmp(command[i + 1], "||") == 0)
                        {
                            if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)
                                break; // Stop executing subsequent commands
                        }
                    }
                }
            }
        }
    }

    // Wait for the last background process to complete
    if (bg_pid > 0)
    {
        if (waitpid(bg_pid, &wstatus, 0) < 0)
        {
            perror("Error waitpid");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}