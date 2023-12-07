#include "../include/myps.h"

void print_process_info(int pid)
{
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *status_file = fopen(path, "r");
    if (!status_file)
        handle_error("fopen (status_file)", -1);

    char line[256];
    int process_id;
    int uid;
    int parent_id;
    char stat[3];
    char name[32];

    while (fgets(line, sizeof(line), status_file) != NULL)
    {
        if (strncmp(line, "Pid:", 4) == 0)
            sscanf(line, "Pid: %d", &process_id);
        else if (strncmp(line, "PPid:", 5) == 0)
            sscanf(line, "PPid: %d", &parent_id);
        else if (strncmp(line, "State:", 5) == 0)
            sscanf(line, "State: %s", stat);
        else if (strncmp(line, "Name:", 5) == 0)
            sscanf(line, "Name: %s", name);
        else if (strncmp(line, "Uid:", 4) == 0)
            sscanf(line, "Uid: %d", &uid);
    }
    fclose(status_file);

    char cmdline_path[256];
    snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);

    FILE *cmdline_file = fopen(cmdline_path, "r");
    if (!cmdline_file)
        handle_error("fopen (cmdline)", -1);

    char cmdline[32];
    if (fgets(cmdline, sizeof(cmdline), cmdline_file) == NULL)
        handle_error("fgets (cmdline)", -1);

    printf("%-10d %-10d %-10d %-10s %-20s %-30s\n", uid, process_id, parent_id, stat, name, cmdline);
}

int main(int arc, char *argv[])
{
    DIR *proc_dir = opendir("/proc");

    if (!proc_dir)
        handle_error("opendir", -1);

    printf("%-10s %-10s %-10s %-10s %-20s %-30s\n", "UID", "PID", "PPID", "STAT", "NAME", "COMMAND");

    struct dirent *entry;

    while ((entry = readdir(proc_dir)) != NULL)
    {
        if (entry->d_name[0] != '.' && atoi(entry->d_name) > 0)
        {
            int pid = atoi(entry->d_name);
            if (pid > 0)
                print_process_info(pid);
        }
    }
    closedir(proc_dir);

    return 0;
}