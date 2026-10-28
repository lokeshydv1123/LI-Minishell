#include "main.h"

int main(void)
{
    system("clear");

    char mini[100] = "thor@son_of_odin$ ";
    char str[1024];

    while (1)
    {
        printf("%s", mini);
        fflush(stdout);

        if (fgets(str, sizeof(str), stdin) == NULL)
            break;

        str[strcspn(str, "\n")] = '\0';

        if (strncmp(str, "PS1", 3) == 0)
            copy_change(mini, str);

        else if (str[0] != '\0')
        {
            char command_copy[1024];
            strcpy(command_copy, str);
            char *command = get_command(command_copy);
            int type = check_command_type(command);

            switch (type)
            {
                case BUILTIN:
                    printf("Built-in command: %s\n", command);
                    break;

                case EXTERNAL:
                    printf("External command: %s\n", command);
                    break;

                case NO_COMMAND:
                    printf("Command not found: %s\n", command != NULL ? command : "");
                    break;
            }
        }
    }

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