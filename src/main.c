#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>
#include <fnmatch.h>
#include <dirent.h>

#include "../include/local_variables_handler.h"
#include "../include/token_handler.h"

#define COMMAND_LENGTH 2048

#define ERR -1
#define NAME_FILE "/bin/bash"
#define NUM_SHM 1664
#define NUM_MUTEX 31

int mutex, shmid;
key_t shm_key;
struct Variable *global_variables;

struct sembuf P = {0, -1, 0};
struct sembuf V = {0, +1, 0};

// BUG: Only one TTY must shmdt and shmctl on shared memory.
//  We must ensure that the TTY that calls kill_handler is the last to use the shared memory
void kill_handler(int n)
{
    struct sembuf remove = {0, 1, IPC_RMID};

    // Remove the semaphore
    if (semop(mutex, &remove, 1) == -1)
        handle_error("Problem during detachment of the mutex", -1);

    // Detach shared memory
    if (shmdt(global_variables) == -1)
        handle_error("Problem during detachment of shared memory", -1);

    // Remove shared memory
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        handle_error("Problem during removal of shared memory", -1);

    exit(EXIT_SUCCESS);
}

void setup_shared_memory()
{
    // Create or obtain the key for shared memory
    if ((shm_key = ftok(NAME_FILE, NUM_SHM)) == ERR)
        handle_error("ftok shared memory", -1);

    // Create or obtain the ID of the shared memory segment
    if ((shmid = shmget(shm_key, MAX_VARIABLES * sizeof(struct Variable), 0666 | IPC_CREAT)) == -1)
        handle_error("shmget", -1);

    // Attach shared memory
    if ((global_variables = shmat(shmid, NULL, 0)) == (void *)-1)
        handle_error("shmat", -1);
}


void executeCommandWithWildcards(char *pattern, char *command, char **args) {
    DIR *dir = opendir(".");
    int i = 0;
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(pattern, entry->d_name,FNM_PATHNAME) == 0) {
            printf("Matching file: %s \n", entry->d_name);
            // Construire le chemin complet du fichier
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "./%s", entry->d_name);

            // Créer un processus enfant pour exécuter la commande
            pid_t pid = fork();
            if (pid == -1) {
                perror("Error fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Exécuter la commande avec le fichier correspondant
                while (args[i] != NULL){
                    i+=1;
                }
                args[i-1] = filepath;//XXX moyen une erreur valgrind
                execvp(args[0], args);
                perror("Error execvp");
                exit(EXIT_FAILURE);
            } else {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    printf("Command exited with status: %d\n", WEXITSTATUS(status));
                }
            }
        }
    }

    closedir(dir);
}

/**
 * @brief Executes commands using execvp.
 * @param mask `int *` Bitmask indicating options for the myls command.
 * @param args `char **` Array containing command arguments.
 */
void execute_command(int *mask, char *args[], struct Variable *variables, struct Variable *global_variables, int mutex)
{
    // Check for wildcard characters in the command
    char *wildcard_chars = "*?";
    if (strpbrk(args[0], wildcard_chars) != NULL) {
        // If wildcard characters are present, use executeCommandWithWildcards
        executeCommandWithWildcards(args[0], args[0], args);
        return;
    }
    else if (strcmp(args[0], "myls") == 0) // Check if the command is "myls"
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
        if (args[1] != NULL && args[2] != NULL && args[3] != NULL)
        {
            if (semop(mutex, &P, 1) == -1)
                handle_error_noexit("Problem during P operation on the mutex");
            set_local_variable(global_variables, args[1], args[3]);
            if (semop(mutex, &V, 1) == -1)
                handle_error_noexit("Problem during V operation on the mutex");
        }
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        if (semop(mutex, &P, 1) == -1)
            handle_error_noexit("Problem during P operation on the mutex");
        print_variable(global_variables, args[1]);
        if (semop(mutex, &V, 1) == -1)
            handle_error_noexit("Problem during V operation on the mutex");

        /*if (args[1][0] == '$')
            print_variable(variables, args[1]);
        else if (execvp(args[0], args) == -1)
            handle_error_noexit("Command execution error");*/
    }
    else if (execvp(args[0], args) == -1) // If it's neither "myls" nor "myps", execute the command using execvp
        handle_error_noexit("Command execution error");
}

int main()
{
    key_t k;

    signal(SIGINT, kill_handler);

    setup_shared_memory();

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

    // struct Variable *variables = (struct Variable *)calloc(MAX_VARIABLES, sizeof(struct Variable));
    struct Variable variables[MAX_VARIABLES];

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
                        execute_command(&mask, args, variables, global_variables, mutex);
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
                        execute_command(&mask, args, variables, global_variables, mutex);
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

    // Detach shared memory
    if (shmdt(global_variables) == -1)
        handle_error("Problem during detachment of shared memory", -1);

    // Remove shared memory
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        handle_error("Problem during removal of shared memory", -1);

    return 0;
}