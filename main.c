#include "main.h"

#define MAX_EXTERNAL_COMMANDS 200

// Function to extract external commands from a file and store them in an array
pid_t pid = -1;
int status = 0;
char *prompt;

Job *head = NULL;

int main(void)
{
    // Initialize the prompt and input string
    char mini[100] = "minishell$ ";
    char str[1024];
    prompt = mini;
    char *external_commands[MAX_EXTERNAL_COMMANDS];

    // Extract external commands from the file and store them in the external_commands array
    extract_external_commands(external_commands);

    // Set up signal handlers for SIGINT, SIGTSTP, and SIGCHLD
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);

    // Main loop to continuously read and process user input
    while (1)   
    {
        printf("%s", mini);
        fflush(stdout);

        if (fgets(str, sizeof(str), stdin) == NULL)
        {   
            if (feof(stdin))
            break;

            if(ferror(stdin))
            {
                clearerr(stdin);
                //printf("\n");
                continue;
            }
        
        }

    // Remove the trailing newline character from the input string
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

    // Execute the command based on its type (BUILTIN, EXTERNAL, or NO_COMMAND)
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
            char external_copy[1024];
            strcpy(external_copy, str);
            execute_external_commands(external_copy);
            break;
        }

        case NO_COMMAND:
            printf("Command not found: %s\n",
                   command != NULL ? command : "");
            break;
    }
}

    // Free the memory allocated for external commands before exiting the program
    for (int i = 0; external_commands[i] != NULL; i++)
        free(external_commands[i]);

    return 0;

}

// Validation function to change the prompt
void copy_change(char *prompt, char *input_string)
{   
    int i;

    // Check for spaces or tabs in the input string
    for (i = 0; input_string[i] != '\0'; i++)
    {
        if (input_string[i] == ' ' || input_string[i] == '\t')
        {
            printf("Error: Spaces/tabs are not allowed in the PS1 command\n");
            return;
        }
    }

    // Check if the input string starts with "PS1=" (case-sensitive)
    if (strncmp(input_string, "PS1=", 4) != 0)
    {
        printf("Error: Use PS1 in uppercase\n");
        return;
    }

    // Check if the new prompt is empty after "PS1="
    if (input_string[4] == '\0')
    {
        printf("Error: Enter a valid prompt value\n");
        return;
    }

    // Check if the new prompt exceeds the maximum allowed length
    if (strlen(input_string + 4) + 3 > 100)
    {
        printf("Error: Prompt is too long\n");
        return;
    }

    strcpy(prompt, input_string + 4);

    int len = strlen(prompt);

    // If the prompt already ends with '$', append only a space.
    if (len > 0 && prompt[len - 1] == '$')
        strcat(prompt, " ");
    else
        strcat(prompt, "$ ");

}