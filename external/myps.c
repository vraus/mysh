#include "../include/myps.h"

int main(int arc, char *argv[])
{

    DIR *proc_dir = opendir("/proc");

    if (!proc_dir)
        handle_error("opendir", -1);
}