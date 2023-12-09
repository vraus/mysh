#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>

#include "../include/local_variables_handler.h"
#include "../include/token_handler.h"

#define COMMAND_LENGTH 2048

#define ERR -1
#define NAME_FILE "/bin/bash"
#define NUM_SHM 1664
#define NUM_MUTEX 31

struct sembuf P = {0, -1, 0};
struct sembuf V = {0, +1, 0};

/**
 * @brief Executes commands using execvp.
 * @param mask `int *` Bitmask indicating options for the myls command.
 * @param args `char **` Array containing command arguments.
 */
void execute_command(int *mask, char *args[], struct Variable *variables, int mutex)
{
    if (strcmp(args[0], "myls") == 0) // Check if the command is "myls"
    {
        hasOption(args, mask); // Parse options for the "myls" command
        is_myls(mask);         // Execute the "myls" command with specified options
    }
    else if (strcmp(args[0], "myps") == 0) // Check if the command is "myps"
        is_myps();                         // Execute the "myps" command
    else if (strcmp(args[0], "set") == 0)
    {
        if (args[1] != NULL && args[2] != NULL && args[3] != NULL)
            set_local_variable(variables, args[1], args[3]);
        else
            handle_error_noexit("set: Bad usage: set <name>=<value>");
    }
    else if (strcmp(args[0], "unset") == 0)
    {
        if (args[1] != NULL)
            unset_local_variables(variables, args[1]);
        else
            handle_error_noexit("unset: Bad usage: unset <name>");
    }
    else if (strcmp(args[0], "setenv") == 0)
    {
        if (semop(mutex, &P, 1) == -1)
            handle_error_noexit("Problem during P operation on the mutex");
        printf("mutex allow you to talk.\n");
        if (semop(mutex, &V, 1) == -1)
            handle_error_noexit("Problem during V operation on the mutex");
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        if (args[1][0] == '$')
            print_variable(variables, args[1]);
        else if (execvp(args[0], args) == -1)
            handle_error_noexit("Command execution error");
    }
    else if (execvp(args[0], args) == -1) // If it's neither "myls" nor "myps", execute the command using execvp
        handle_error_noexit("Command execution error");
}

int main()
{
    key_t key = ftok("/tmp", 's'), k;
    int shmid = shmget(key, MAX_VARIABLES * sizeof(struct Variable), 0666 | IPC_CREAT);
    int mutex;

    if ((k = ftok(NAME_FILE, NUM_MUTEX)) == ERR)
        handle_error("ftok mutex", -1);

    // Attempts to create the mutex
    if ((mutex = semget(k, 1, 0666 | IPC_CREAT | IPC_EXCL)) != ERR)
    {
        // The mutex was created, initialize it with the value 1
        if (semctl(mutex, 0, SETVAL, 1) == ERR)
            handle_error("Problem during initialization of semaphore mutex", -1);
    }
    else if (errno == EEXIST)
    {
        // Semaphore already exists, obtain it without reinitializing
        if ((mutex = semget(k, 1, 0666)) == ERR)
            handle_error("Problem obtaining mutex", -1);
    }
    else
        handle_error("Probleme during mutex creation", -1);

    if (shmid == -1)
        handle_error("shmget", -1);

    struct Variable *global_variables = shmat(shmid, NULL, 0);
    if ((long)global_variables == -1)
        handle_error("shmat", -1);

    struct Variable *variables = (struct Variable *)calloc(MAX_VARIABLES, sizeof(struct Variable));

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
                        execute_command(&mask, args, variables, mutex);
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
                        execute_command(&mask, args, variables, mutex);
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

    if (semctl(mutex, 1, IPC_RMID) == -1)
        handle_error("Problem during detachment of the mutex", -1);

    return 0;
}