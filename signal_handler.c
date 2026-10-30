#include "main.h"

void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTSTP)
    {
        if (pid <= 0)
        {
            write(STDOUT_FILENO, "\n", 1);
        }
    }
    else if (signum == SIGCHLD)
    {
        while (waitpid(-1, &status, WNOHANG) > 0)
        {
            /* Reap completed child processes */
        }
    }
}