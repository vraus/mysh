# MySH - M1 SE

> **Authors**: *Ugo Courche (#BaKeMoNo0)* & *Hadrien Belleville (#Vraus)*
>
> **Date of the first commit**: *October 11, 2023*
>
> **Contributions**:
> |                      | Commits | Additions | Deletions | Global |
> |:-------------------- |:-------:|:---------:|:---------:|-------:|
> | **Ugo Courche**      |20 (26%) |1530 (43%) |479 (29%)  |**33%** |
> |**Hadrien Belleville**|58 (74%) |2060 (57%) |1199 (71%) |**67%** |
> | **Total**            |  **78** |  **3590** |  **1678** |**100%**|

## Table of Content

1. [How to Use](#how-to-use)

1. [Features](#features)

    1. [Implemented Features](#implemented)

    1. [Non-Implemented Features](#non-implemented)

1. [Bugs](#bugs)

1. [Workspace Organization](#workspace-organization)

    1. [Plugins](#plugins)

1. [Scripts](#scripts)

    1. [Background Test Script](#background-test-script)

## How to Use

In order to use the `MySH` at the current state, you only have to run the following command.

```bash
make && ./main
```

You will then be able to access the prompt of the mysh. You don't have a lot of usable commands at this state of the project. Only the most basic UNIX commands you can think of, and the `myls` command that is supposed to work as a normal `ls` command.

## Features

### Implemented

#### 2.1 Sequencing commands

In our code, the `main` function reads the user's input, tokenizes it into individual commands, and then iterates through each command. For each command, a child process is forked, and the command is executed using `execvp`.

The parent process `waits` for the child to finish before proceeding to the next command. The logic also handles logical operators (`&&`, `||`) to determine whether to execute subsequent commands based on the success or failure of the previous command.

#### 2.2 Wildcards

The wildcard handling is implemented in the `execute_command_with_wildcards` function in the `wildcard_handler.c` file. It uses the `fnmatch` function to match the given pattern with the files in the current directory. If a match is found, a new process is forked to execute the command with the matched file as an argument.

#### 3.4 myls

The `myls` functionality is implemented as an external program in the `myls.c` file.

It opens the current directory, retrieves information about files and directories, and prints the details in a format similar to `ls -l`.

It also supports `-a` option for displaying hidden files, but `-R`, for recursive listing, does not works.

#### 3.5 myps

The `myps` functionality is implemented as an external program in the `myps.c` file.

It reads information about each process from the `/proc/<pid>/status` and `/proc/<pid>/cmdline` files and prints following details: `UID`, `PID`, `PPID`, `STAT`, `NAME`, and `COMMAND`.

The main function in `myps.c` opens the `/proc` directory, iterates through process entries, and calls `print_process_info` for each process.

#### 5 Background Command

The code supports running commands in the background if the user appends an `&` at the end of the command. This is achieved by forking a child process to execute the background command, while the parent continues to read and execute subsequent commands.

To demonstrate the functionality, we've created a [background test script](#background-test-script) you can use by running `./test_script.sh &` in the tinyshell.

#### 6.1 Global Variables

Global variables are implemented using shared memory. The `setup_shared_memory` function in the main function creates or obtains a key for shared memory, creates or obtains the ID of the shared memory segment, and attaches the shared memory.

Global variables are then accessed through the `global_variables` pointer.

#### 6.2 Local Variables

Local variables are implemented using an array of `struct Variable` in the `local_variables_handler.c` file. Functions such as `set_local_variable` and `unset_local_variables` modify this array based on user commands.

#### 6.3 Usage of Variables

Variables are used in the code to store and retrieve values. For example, in the `set_local_variable` function, if the value of a variable is another variable (e.g., `$var`), the code retrieves the value of `var` and assigns it to the new variable.

You can for example use `echo $a` which will print the value inside of variable `a`.

### Non-Implemented

> Following, the list of non-implemented features in our tinyshell

+ Changing Directories

+ 3.3 Return code of processus

+ 4.2 Redirection to or from a file

+ 5.1 `myjobs`

+ 5.2 `Ctrl-Z`

## Bugs

### 3.1 Changing Directories

As we did not make any external program to handle a `mycd`, any `cd` command is handled with `execvp` which doesn't work for this specific command inside the tinyshell.

### 3.2 Exiting with `Ctrl-C`

Exiting the tinyshell with `Ctrl-C` is allowed, threads, semaphores and shared memory are badly closed when using a `SIGINT`.

The `kill_handler` function in the `main.c` file tryes to access the global `sem`, `shmd` and pthreads, but doesn't seem to succeed at closing and joining them.

### 4.1 Pipes

## Workspace Organization

### Plugins

#### Todo Tree

> from Gruntfuggly, [zaonce.com](https://www.zaonce.com/cgi-bin/blog.pl)

We use this extension available on the marketplace of VSCode to organize the workspace with TODO tags.

Simple documentation on how to use TODO comment and configure in the workspace can be found [here](https://thomasventurini.com/articles/the-best-way-to-work-with-todos-in-vscode/).

Also interesting if we want to use icons, the list of all supported icons can be found [here](https://microsoft.github.io/vscode-codicons/dist/codicon.html).

## Scripts

### Background test script

The purpose of this test script is to demonstrate the functionality of the `&` command in our terminal. The script is straightforward and involves a 10-second sleep before printing "Awake!" on the screen.

```bash
#!/bin/bash

echo "Start sleeping for 10 seconds"
sleep 10
echo "Awake!"
```
