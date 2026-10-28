#include "main.h"

void scan_input(char *prompt, char *input_string)
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

        default:
            printf("Command not found: %s\n", command != NULL ? command : "");
            break;
    }
}

char *get_command(char *input_string)
{
    char *command;
    command = strtok(input_string, " \t");
    return command;
}

int check_command_type(char *command)
{
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","jobs","fg","bg" NULL }; // implement these -> cd pwd exit echo
        // chdir(path) for the cd command
        // getcwd for the pwd command
        // exit(0) for terminating the entire project
        // echo is like printf to print some content    --> echo $SHELL(getenv), echo $$(getpid()), echo $?(WEXITSTATUS(status)), 
        // echo hello -> directly print that string if its not starting with $

    char *external_commands[] = {"bash", "bunzip2", "busybox", "bzcat", "bzcmp", "bzdiff", "bzegrep", "bzexe", "bzfgrep", "bzgrep",
        "bzip2", "bzip2recover", "bzless", "bzmore", "cat", "chacl", "chgrp", "chmod", "chown", "chvt", "cp", "cpio", "dash", "date",
        "dbus-cleanup-sockets", "dbus-daemon", "dbus-uuidgen", "dd", "df", "dir", "dmesg", "dnsdomainname", "domainname", "dumpkeys", "ed", 
        "egrep", "false", "fgconsole", "fgrep", "findmnt", "fuser", "fusermount","getfacl", "grep", "gunzip", "gzexe", "gzip", "hostname", "ip",
        "kbd_mode", "kill", "kmod","less", "lessecho", "lessfile", "lesskey", "lesspipe","ln", "loadkeys", "login", "loginctl", "lowntfs-3g",
        "ls", "lsblk", "lsmod", "mkdir", "mknod", "mktemp", "more", "mount", "mountpoint", "mt", "mt-gnu", "mv", "nano", "nc", "nc.openbsd", 
        "netcat", "netstat", "nisdomainname", "ntfs-3g", "ntfs-3g.probe", "ntfs-3g.secaudit", "ntfs-3g.usermap", "ntfscat",
        "ntfsck", "ntfscluster", "ntfscmp", "ntfsdump_logfile", "ntfsfix", "ntfsinfo", "ntfsls", "ntfsmftalloc","ntfsmove", "ntfstruncate", 
        "ntfswipe", "open", "openvt", "pidof", "ping", "ping6", "plymouth", "plymouth-upstart-bridge", "ps", "rbash", "readlink",
        "red", "rnano", "rmdir", "run-parts", "sed", "setfacl", "setfont", "setupcon", "sh", "sh.distrib","sleep", "ss", "static-sh", 
        "stty", "su", "sync", "tailf", "tar", "tempfile", "touch", "udevadm", "ulockmgr_server", "umount", "uname", "uncompress",
        "unicode_start", "vdir", "vmmouse_detect", "which", "whiptail", "ypdomainname", "zcat", "zcmp", "zdiff","zegrep", "zfgrep", 
        "zforce", "zgrep", "zless", "zmore", "znew", NULL };

    int i = 0;
    if (command == NULL || command[0] == '\0')
        return NO_COMMAND;

    while (builtins[i] != NULL)
    {
        if (strcmp(command, builtins[i]) == 0)
            return BUILTIN;

        i++;
    }

    i = 0;
    while (external_commands[i] != NULL)
    {
        if (strcmp(command, external_commands[i]) == 0)
            return EXTERNAL;

        i++;
    }

    return NO_COMMAND;
}



