#include "main.h"

#define MAX_EXTERNAL_COMMANDS 200
pid_t pid = -1;
int status = 0;
Job *head = NULL;

int main(void)
{
    char mini[100] = "thor@son_of_odin$ ";
    char str[1024];

    char *external_commands[MAX_EXTERNAL_COMMANDS];

    extract_external_commands(external_commands);

    while (1)
    {
        printf("%s", mini);
        fflush(stdout);

        if (fgets(str, sizeof(str), stdin) == NULL)
            break;

        str[strcspn(str, "\n")] = '\0';

        if (strncmp(str, "PS1", 3) == 0)
        {
            copy_change(mini, str);
            continue;
        }

        if (str[0] == '\0')
            continue;

        char command_copy[1024];
        strcpy(command_copy, str);

        char *command = get_command(command_copy);

        int type = check_command_type(command, external_commands);

        switch (type)
        {
            case BUILTIN:
            {
                char internal_copy[1024];
                strcpy(internal_copy, str);

                execute_internal_commands(internal_copy);
                break;
            }

            case EXTERNAL:
            {
                char command_copy[1024];
                strcpy(command_copy, str);

                execute_external_commands(command_copy);
                break;
            }

            case NO_COMMAND:
                printf("Command not found: %s\n",
                       command != NULL ? command : "");
                break;
        }
    }

    for (int i = 0; external_commands[i] != NULL; i++)
        free(external_commands[i]);

    return 0;
}

void copy_change(char *prompt, char *input_string)
{   
    int i;

    for (i = 0; input_string[i] != '\0'; i++)
    {
        if (input_string[i] == ' ' || input_string[i] == '\t')
        {
            printf("Error: Spaces/tabs are not allowed in the PS1 command\n");
            return;
        }
    }

    if (strncmp(input_string, "PS1=", 4) != 0)
    {
        printf("Error: Use PS1 in uppercase\n");
        return;
    }

    if (input_string[4] == '\0')
    {
        printf("Error: Enter a valid prompt value\n");
        return;
    }

    if (strlen(input_string + 4) + 3 > 100)
    {
        printf("Error: Prompt is too long\n");
        return;
    }

    strcpy(prompt, input_string + 4);
    strcat(prompt, "$ ");
}