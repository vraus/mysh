#include "../include/wildcard_handler.h"

void execute_command_with_wildcards(char *pattern, char *command, char **args)
{
    printf("%s\n", command);
    DIR *dir = opendir(".");
    int i = 0;
    if (dir == NULL)
    {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (fnmatch(pattern, entry->d_name, FNM_PATHNAME) == 0)
        {
            printf("Matching file: %s \n", entry->d_name);
            // Construire le chemin complet du fichier
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "./%s", entry->d_name);

            // Créer un processus enfant pour exécuter la commande
            pid_t pid = fork();
            if (pid == -1)
            {
                perror("Error fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                // Exécuter la commande avec le fichier correspondant
                while (args[i] != NULL)
                {
                    i += 1;
                }
                args[i - 1] = filepath; // XXX moyen une erreur valgrind
                execvp(args[0], args);
                perror("Error execvp");
                exit(EXIT_FAILURE);
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status))
                {
                    printf("Command exited with status: %d\n", WEXITSTATUS(status));
                }
            }
        }
    }

    closedir(dir);
}