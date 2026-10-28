# Mini Shell in C

A command-line shell implemented in C that demonstrates Linux process management, external command execution, built-in commands, and inter-process communication using pipes.

## Overview

This project implements a basic shell that accepts user commands, identifies built-in and external commands, and executes them. It also supports pipelines, allowing the output of one command to serve as the input of another.

The project is being developed incrementally to understand Linux system calls, process creation, signal handling, and shell internals.

## Features

### 1. Custom Command Prompt

* Displays a custom shell prompt.
* Supports changing the prompt using the `PS1` variable.
* Validates the prompt input before updating it.

Example:

```bash
PS1=loki
```

### 2. Built-in Commands

The shell implements the following built-in commands:

| Command | Description                                                                |
| ------- | -------------------------------------------------------------------------- |
| `cd`    | Changes the current working directory.                                     |
| `pwd`   | Displays the current working directory.                                    |
| `echo`  | Prints text and supports selected environment and shell-related variables. |
| `exit`  | Terminates the shell.                                                      |

### 3. External Command Execution

* Maintains a list of supported external commands in `external_commands.txt`.
* Loads the command list from the file.
* Identifies external commands entered by the user.
* Creates child processes using `fork()`.
* Executes external programs using `execvp()`.
* Uses `waitpid()` to wait for foreground processes.

Examples:

```bash
ls
ls -l
date
cat external_commands.txt
```

### 4. Pipeline Support

Supports connecting multiple external commands using pipes (`|`).

The shell uses `pipe()` and `dup2()` to redirect standard input and output between processes.

Examples:

```bash
ls | wc -l
ls | grep ".c"
cat external_commands.txt | grep ls | wc -l
cat external_commands.txt | grep ls | sort | wc -l
```

### 5. Initial Job Control Implementation

The project includes initial implementations for managing process information using a linked list.

Planned job-control features include:

* `jobs` — displays stored process information.
* `bg` — resumes a stopped process in the background.
* `fg` — resumes a stopped process and waits for it in the foreground.
* Signal handling for process control.

**Note:** Complete job control, including signal handling and foreground/background terminal management, is still under development.

## Technologies and Concepts

* **Language:** C
* **Platform:** Linux / Linux-compatible environments
* **System calls and APIs:** `fork()`, `execvp()`, `pipe()`, `dup2()`, `waitpid()`, `chdir()`, `getcwd()`, `kill()`
* **Signals:** `SIGINT`, `SIGTSTP`, `SIGCONT`, `SIGCHLD`
* **Data structures:** Singly linked list
* **Inter-process communication:** Pipes
* **File handling:** Reading external command names from a text file
* **Memory management:** Dynamic memory allocation

## Project Structure

```text
minishell_project/
├── main.c
├── main.h
├── scan_input.c
├── extract_external_commands.c
├── external_commands.txt
├── execute_external_commands.c
├── execute_internal_commands.c
└── README.md
```

*The filenames above describe the intended organization; adjust them to match the files present in your repository.*

## Compilation

Compile the source files using GCC:

```bash
gcc -Wall -Wextra *.c -o minishell
```

## Running the Shell

Start the shell:

```bash
./minishell
```

Try the following commands:

```bash
pwd
ls -l
echo hello
date
ls | wc -l
exit
```

## Learning Objectives

This project provides practical experience with:

* Command parsing and input validation.
* Distinguishing built-in commands from external programs.
* Creating and managing processes.
* Executing programs using `execvp()`.
* Redirecting input and output using file descriptors.
* Implementing pipelines with multiple processes.
* Managing process information with linked lists.
* Understanding Unix signals and job control.

## Future Improvements

* Complete `jobs`, `fg`, and `bg` functionality.
* Implement robust signal handling.
* Improve foreground and background process management.
* Add command validation and error handling.
* Support additional shell features as required.

## Author

Developed as a systems programming project to explore Linux process management and shell implementation using C.
