#include "main.h"

void extract_external_commands(char **external_commands)
{
    FILE *fp = fopen("external_commands.txt", "r");

    if (fp == NULL)
    {
        perror("external_commands.txt");
        exit(EXIT_FAILURE);
    }

    char command[128];
    int i = 0;

    while (fgets(command, sizeof(command), fp) != NULL)
    {
        command[strcspn(command, "\r\n")] = '\0';

        if (command[0] == '\0')
            continue;

        external_commands[i] = malloc(strlen(command) + 1);

        if (external_commands[i] == NULL)
        {
            perror("malloc");
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        strcpy(external_commands[i], command);

        i++;
    }

    external_commands[i] = NULL;

    fclose(fp);
}

void execute_external_commands(char *input_string)
{
    char *commands[100];
    char *args[100];
    int pipefd[2];
    int prev_read = -1;
    int count = 0;
    int i = 0;

    char *saveptr;
    commands[count] = strtok_r(input_string, "|", &saveptr);

    while (commands[count] != NULL && count < 99)
    {
        count++;
        commands[count] = strtok_r(NULL, "|", &saveptr);
    }

    if (count == 0)
        return;

    pid_t pids[count];

    for (int j = 0; j < count; j++)
    {
        while (*commands[j] == ' ' || *commands[j] == '\t')
            commands[j]++;

        int len = strlen(commands[j]);

        while (len > 0 && (commands[j][len - 1] == ' ' || commands[j][len - 1] == '\t'))
            commands[j][--len] = '\0';

        if (commands[j][0] == '\0')
        {
            fprintf(stderr, "Invalid null command in pipeline\n");
            if (prev_read != -1)
                close(prev_read);
            return;
        }

        if (j < count - 1)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                if (prev_read != -1)
                    close(prev_read);
                return;
            }
        }

        pid_t child = fork();

        if (child == -1)
        {
            perror("fork");

            if (prev_read != -1)
                close(prev_read);

            if (j < count - 1)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            return;
        }

        if (child == 0)
        {
            if (prev_read != -1)
            {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }

            if (j < count - 1)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            char *arg_saveptr;
            i = 0;
            args[i] = strtok_r(commands[j], " \t", &arg_saveptr);

            while (args[i] != NULL && i < 98)
            {
                i++;
                args[i] = strtok_r(NULL, " \t", &arg_saveptr);
            }

            if (args[0] == NULL)
                exit(EXIT_FAILURE);

            execvp(args[0], args);

            perror(args[0]);
            exit(EXIT_FAILURE);
        }

        pids[j] = child;

        if (prev_read != -1)
            close(prev_read);

        if (j < count - 1)
        {
            close(pipefd[1]);
            prev_read = pipefd[0];
        }
    }

    for (int j = 0; j < count; j++)
    {
        int child_status;

        if (waitpid(pids[j], &child_status, 0) == -1)
        {
            perror("waitpid");
            continue;
        }

        if (j == count - 1)
        {
            if (WIFEXITED(child_status))
                status = WEXITSTATUS(child_status);
            else if (WIFSIGNALED(child_status))
                status = 128 + WTERMSIG(child_status);
        }
    }
}