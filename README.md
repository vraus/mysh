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

#### 2.2 Wildcards

#### 3.4 myls

#### 3.5 myps

#### 5 Background Command

#### 6.1 Global Variables

#### 6.2 Local Variables

#### 6.3 Usage of Variables

### Non-Implemented

#### 3.3 Return code of processus

#### 4.2 Redirection to or from a file

#### 5.1 `myjobs`

#### 5.2 `Ctrl-Z`

## Bugs

### 3.1 Changing Directories

### 3.2 Exiting with `Ctrl-C`

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
