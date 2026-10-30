#include "main.h"

// Function to extract external commands from a file and store them in an array
void extract_external_commands(char **external_commands)
{
    FILE *fp = fopen("external_commands.txt", "r");  // Open the file containing external commands

    if (fp == NULL)
    {
        perror("external_commands.txt");
        exit(EXIT_FAILURE);
    }

    // Read each line from the file and store it in the external_commands array
    char command[128];
    int i = 0;

    // Read each line from the file and store it in the external_commands array
    while (fgets(command, sizeof(command), fp) != NULL)
    {
        command[strcspn(command, "\r\n")] = '\0';

        if (command[0] == '\0')
            continue;

        external_commands[i] = malloc(strlen(command) + 1);  // Allocate memory for the command string

        // Check if memory allocation was successful
        if (external_commands[i] == NULL)
        {
            perror("malloc");
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        strcpy(external_commands[i], command);  // Copy the command string into the allocated memory

        i++;
    }

    external_commands[i] = NULL;   // Set the last element of the array to NULL to indicate the end of the commands

    fclose(fp);
}

// Function to execute external commands, including support for pipelines
void execute_external_commands(char *input_string)
{
    char *commands[100];
    char *args[100];
    int pipefd[2];
    int prev_read = -1;
    int count = 0;
    char *saveptr;

    /* Validate pipe usage before splitting commands */
    if (input_string[0] == '|' || input_string[strlen(input_string) - 1] == '|')
    {
        printf("Error: Invalid pipe usage. Please enter a valid command.\n");
        return;
    }

    if (strstr(input_string, "||") != NULL)
    {
        printf("Error: Invalid pipe usage. Please enter a valid command.\n");
        return;
    }

    // Split the input string into individual commands based on the '|' character
    commands[count] = strtok_r(input_string, "|", &saveptr);

    // Continue splitting the input string into commands until there are no more commands or the maximum limit is reached
    while (commands[count] != NULL && count < 99)
    {
        count++;
        commands[count] = strtok_r(NULL, "|", &saveptr);
    }

    if (count == 0)
        return;

    pid_t pids[count];

    // Loop through each command in the pipeline and execute them
    for (int j = 0; j < count; j++)
    {
        while (*commands[j] == ' ' || *commands[j] == '\t')
            commands[j]++;

        int len = strlen(commands[j]);

        while (len > 0 && (commands[j][len - 1] == ' ' || commands[j][len - 1] == '\t'))
        {
            commands[j][--len] = '\0';
        }

        if (commands[j][0] == '\0')
        {
            fprintf(stderr, "Invalid null command in pipeline\n");  

            if (prev_read != -1)
                close(prev_read);

            return;
        }

        // Create a pipe for the current command if it's not the last command in the pipeline
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

        // creating a child process to execute the commands
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

        // In the child process, set up the necessary input/output redirection for the pipeline
        if (child == 0)
        {

            if (prev_read != -1)   // If there is a previous read end of the pipe, redirect it to standard input
            {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }

            if (j < count - 1)   // If this is not the last command in the pipeline, redirect standard output to the write end of the pipe
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // Split the command into arguments for execvp
            char *arg_saveptr;
            int i = 0;

            args[i] = strtok_r(commands[j], " \t", &arg_saveptr);  // Split the command into arguments based on spaces and tabs

            while (args[i] != NULL && i < 98)
            {
                i++;
                args[i] = strtok_r(NULL, " \t", &arg_saveptr);
            }

            // Null-terminate the arguments array
            if (args[0] == NULL)
                exit(EXIT_FAILURE);

            execvp(args[0], args);

            perror(args[0]);
            exit(EXIT_FAILURE);
        }

        // In the parent process, close the write end of the pipe and store the child process ID for later use
        pids[j] = child;

        if (prev_read != -1)
            close(prev_read);

        if (j < count - 1)
        {
            close(pipefd[1]);
            prev_read = pipefd[0];
        }
    }

    // Wait for all child processes in the pipeline to complete and handle their exit statuses
    if (prev_read != -1)
        close(prev_read);

    /* Wait for all pipeline children */
    for (int j = 0; j < count; j++)
    {
        int child_status;

        if (waitpid(pids[j], &child_status, WUNTRACED) == -1)
        {
            perror("waitpid");
            continue;
        }

        // Check if the child process was stopped and insert it into the job list if it was
        if (WIFSTOPPED(child_status))
            insert_job(pids[j], "Stopped", input_string);    
        else if (j == count - 1)
        {
            if (WIFEXITED(child_status))   // If the child process exited normally, update the status variable with its exit status
                status = WEXITSTATUS(child_status);
            else if (WIFSIGNALED(child_status))  // If the child process was terminated by a signal, update the status variable with the signal number
                status = 128 + WTERMSIG(child_status);
        }
    }

    pid = -1;
}