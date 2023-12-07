# MySH - M1 SE

> **Authors**: *Ugo Courche (#BaKeMoNo0)* & *Hadrien Belleville (#Vraus)*
>
> **Date of the first commit**: *October 11, 2023*

## Table of Content

1. [How to Use](#how-to-use)

    1. [V-0.1.111023](#v-01111023)

    1. [V-0.2.071223](#v-02171223)

1. [Workspace Organization](#workspace-organization)

    1. [Plugins](#plugins)

1. [Scripts](#scripts)

    1. [Background Test Script](#background-test-script)

## How to Use

> How to read the versions name: V-X.Y.Z
>
> + X: This indicates the major version. It is incremented for significant changes that may introduce backward-incompatible features.
> + Y: This is the minor version. It is typically incremented for smaller, backward-compatible changes or feature additions.
> + Z: This represents the patch version. It is used for bug fixes or small improvements that are backward-compatible. In our case this will always be the date of the first commit of the patch version.

### V-0.1.111023

In order to use the `MySH` at the current state, you only have to run the following command.

```bash
make && ./main
```

You will then be able to access the prompt of the mysh. You don't have a lot of usable commands at this state of the project. Only the most basic UNIX commands you can think of, and the `myls` command that is supposed to work as a normal `ls` command.

### V-0.2.171223

This new version allows you to use commun logical characters like in a everyday terminal. Using `&&` will execute the second part if the first part succeed, and `||` will execute the second part if the first one fails.

Also implemented `&` to execute the command as background command. To test this you can, while in the tiny-shell, you can use our [test script](#background-test-script) using the following command:

```bash
/mysh > ./test_script.sh & echo "hello"
```

And a brand new external program: the `myps` callable with:

```bash
/mysh > myps
```

There are no args for this program. Just type this to have a lighter for collorful version of the `ps aux` command.

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
