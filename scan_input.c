#include "main.h"

void scan_input(char *prompt, char *input_string, char **external_commands)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(input_string, 1024, stdin) == NULL)
    {
        input_string[0] = '\0';
        return;
    }

    input_string[strcspn(input_string, "\n")] = '\0';

    if (input_string[0] == '\0')
        return;

    if (strncmp(input_string, "PS1", 3) == 0)
    {
        copy_change(prompt, input_string);
        return;
    }

    char command_copy[1024];
    strcpy(command_copy, input_string);

    char *args[100];
    int i = 0;

    args[i] = strtok(command_copy, " \t");

    while (args[i] != NULL && i < 99)
    {
        i++;
        args[i] = strtok(NULL, " \t");
    }

    if (args[0] == NULL)
        return;

    int type = check_command_type(args[0], external_commands);

    switch (type)
    {
        case BUILTIN:
            printf("Built-in command: %s\n", args[0]);
            break;

        case EXTERNAL:
        {
            char command_copy[1024];
            strcpy(command_copy, input_string);

            execute_external_commands(command_copy);
            break;
        }

        case NO_COMMAND:
            printf("Command not found: %s\n", args[0]);
            break;
    }
}

char *get_command(char *input_string)
{
    char *command = strtok(input_string, " \t");
    return command;
}

int check_command_type(char *command, char **external_commands)
{
    char *builtins[] = { "echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval", "set", "unset", "export", "declare",
        "typeset", "readonly", "getopts", "source", "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs",
        "fg", "bg", NULL };

    if (command == NULL || command[0] == '\0')
        return NO_COMMAND;

    for (int i = 0; builtins[i] != NULL; i++)
    {
        if (strcmp(command, builtins[i]) == 0)
            return BUILTIN;
    }

    if (external_commands != NULL)
    {
        for (int i = 0; external_commands[i] != NULL; i++)
        {
            if (strcmp(command, external_commands[i]) == 0)
                return EXTERNAL;
        }
    }

    return NO_COMMAND;
}

