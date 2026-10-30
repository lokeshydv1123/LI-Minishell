#include "main.h"

// Function to execute internal commands
void execute_internal_commands(char *input_string)
{
    // Create a copy of the input string to avoid modifying the original
    char input_copy[1024];
    strcpy(input_copy, input_string);

    // Tokenize the input string to extract the command
    char *command = strtok(input_copy, " \t");

    if (command == NULL)
        return;

    // Handle internal commands
    if (strcmp(command, "exit") == 0)
        exit(0);
    else if (strcmp(command, "pwd") == 0)   // Handle the pwd command
    {
        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) != NULL)  // Print the current working directory
            printf("%s\n", cwd);
        else
            perror("getcwd");
    }
    else if (strcmp(command, "cd") == 0)
    {
        char *path = strtok(NULL, " \t");

        if (path == NULL)   // If no path is provided, change to the home directory
            path = getenv("HOME");

        if (path == NULL)  // If HOME is not set, print an error message
            fprintf(stderr, "cd: HOME not set\n");
        else if (chdir(path) == -1)
            perror("cd");
    }
    else if (strcmp(command, "echo") == 0)   // Handle the echo command
        echo(input_string, status);
    else if (strcmp(command, "jobs") == 0)   // Handle the jobs command
        print_jobs();
    else if (strcmp(command, "fg") == 0)    // Handle the fg command
        execute_fg();
    else if (strcmp(command, "bg") == 0)   // Handle the bg command
        execute_bg();
}

// Function to handle the echo command
void echo(char *input_string, int status)
{
    char *text = input_string + 4;

    while (*text == ' ' || *text == '\t')
        text++;

    if (*text == '\0')
        printf("\n");
    else if (strcmp(text, "$SHELL") == 0)
    {
        char *shell = getenv("SHELL");
        printf("%s\n", shell != NULL ? shell : "");
    }
    else if (strcmp(text, "$$") == 0)
        printf("%d\n", (int)getpid());
    else if (strcmp(text, "$?") == 0)
        printf("%d\n", status);
    else
        printf("%s\n", text);
}

// Function to insert a job into the job list
void insert_job(pid_t pid, char *process, char *input)
{
    Job *new = malloc(sizeof(Job));

    if (new == NULL)
    {
        perror("malloc");
        return;
    }

    new->pid = pid;

    strcpy(new->process, process);
    strcpy(new->input, input);

    new->link = head;
    head = new;
}

// Function to delete the first job from the job list
void delete_first_job(void)
{
    if (head == NULL)
        return;

    Job *temp = head;
    head = head->link;

    free(temp);
}

// Function to execute the 'bg' command
void execute_bg(void)
{
    if (head == NULL)
    {
        printf("No stopped processes\n");
        return;
    }

    if (kill(head->pid, SIGCONT) == -1)
    {
        perror("kill");
        return;
    }

    printf("Resumed process: %s\n", head->input);

    delete_first_job();
}

// Function to execute the 'fg' command
void execute_fg(void)
{
    if (head == NULL)
    {
        printf("No stopped processes\n");
        return;
    }

    pid_t fg_pid = head->pid;
    int child_status;

    // Resume the most recently stopped process
    if (kill(fg_pid, SIGCONT) == -1)
    {
        perror("kill");
        return;
    }

    // Wait until the process terminates or stops again
    if (waitpid(fg_pid, &child_status, WUNTRACED) == -1)
    {
        perror("waitpid");
        return;
    }

    // Remove the job only if the process has terminated 
    if (WIFEXITED(child_status) || WIFSIGNALED(child_status))
    {
        delete_first_job();
    }
    else if (WIFSTOPPED(child_status))
    {
        // The process was stopped again using Ctrl+Z. Keep it in the linked list. 
        printf("\nProcess stopped again: %d\n", fg_pid);
    }
}

// Function to print the list of jobs
void print_jobs(void)
{
    Job *temp = head;

    if (temp == NULL)
    {
        printf("No stopped processes\n");
        return;
    }

    printf("PID\tINPUT\n");

    while (temp != NULL)
    {
        printf("%d\t%s\n", temp->pid, temp->input);
        temp = temp->link;
    }
}