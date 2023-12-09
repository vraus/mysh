#include "../include/token_handler.h"

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

void is_myps()
{
    char *args[2];
    args[0] = "./external/myps";
    args[1] = NULL;

    // Execute external "myps" programm
    if (execvp(args[0], args) == -1)
        handle_error("execl (myps)", -1);
}

bool is_space_or_tab(char ch) { return ch == ' ' || ch == '\t'; }