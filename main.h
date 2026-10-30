#ifndef MAIN_H
#define MAIN_H
#define MAX_EXTERNAL_COMMANDS 200

// Global variables
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

extern pid_t pid;
extern int status;
extern char *prompt;

typedef struct node
{
    pid_t pid;
    char process[100];
    char input[1024];
    struct node *link;
} Job;
extern Job *head;

// Command type definitions
#define BUILTIN     1
#define EXTERNAL    2
#define NO_COMMAND  3

// ANSI color codes for terminal output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


// Function declarations
void scan_input(char *prompt, char *input_string, char **external_commands);
char *get_command(char *input_string);
void copy_change(char *prompt, char *input_string);
int check_command_type(char *command, char **external_commands);
void echo(char *input_string, int status);
void execute_internal_commands(char *input_string);
void execute_external_commands(char *input_string);
void signal_handler(int sig_num);
void insert_job(pid_t pid, char *process, char *input);
void delete_first_job(void);
void execute_bg(void);
void execute_fg(void);
void print_jobs(void);
void extract_external_commands(char **external_commands);

#endif