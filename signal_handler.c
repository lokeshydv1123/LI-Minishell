#include "main.h"

//extern char prompt[];
// Signal handler function to handle SIGINT, SIGTSTP, and SIGCHLD signals
void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTSTP)    // Handle SIGINT (Ctrl+C) and SIGTSTP (Ctrl+Z) signals
    {
        if (pid <= 0)
        {
            write(STDOUT_FILENO, "\n", 1);
            //write(STDOUT_FILENO, prompt, strlen(prompt));
        }
    }
    else if (signum == SIGCHLD)    // Handle SIGCHLD signal (child process termination)
    {
        while (waitpid(-1, &status, WNOHANG) > 0)
        {
            /* Reap completed child processes */
        }
    }
}