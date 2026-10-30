#include "main.h"

void execute_internal_commands(char *input_string)
{
    char input_copy[1024];
    strcpy(input_copy, input_string);

    char *command = strtok(input_copy, " \t");

    if (command == NULL)
        return;

    if (strcmp(command, "exit") == 0)
        exit(0);
    else if (strcmp(command, "pwd") == 0)
    {
        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s\n", cwd);
        else
            perror("getcwd");
    }
    else if (strcmp(command, "cd") == 0)
    {
        char *path = strtok(NULL, " \t");

        if (path == NULL)
            path = getenv("HOME");

        if (path == NULL)
            fprintf(stderr, "cd: HOME not set\n");
        else if (chdir(path) == -1)
            perror("cd");
    }
    else if (strcmp(command, "echo") == 0)
        echo(input_string, status);
    else if (strcmp(command, "jobs") == 0)
        print_jobs();
    else if (strcmp(command, "fg") == 0)
        execute_fg();
    else if (strcmp(command, "bg") == 0)
        execute_bg();
}

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

void delete_first_job(void)
{
    if (head == NULL)
        return;

    Job *temp = head;
    head = head->link;

    free(temp);
}

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

void execute_fg(void)
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

    waitpid(head->pid, &status, WUNTRACED);

    delete_first_job();
}

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