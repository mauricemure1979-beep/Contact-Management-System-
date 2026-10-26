#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <process.h>
#include <direct.h>
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif

#define MAX_INPUT 1024
#define MAX_ARGS 64

int parse_input(char *input, char **args)
{
    int i = 0;
    char *token = strtok(input, " \t\n");

    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }

    args[i] = NULL;
    return i;
}

int main(void)
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char cwd[PATH_MAX];

    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s & ", cwd);
        else
            printf("myshell & ");

        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        if (strlen(input) <= 1)
            continue;

        int arg_count = parse_input(input, args);
        if (arg_count == 0)
            continue;

        if (strcmp(args[0], "exit") == 0)
        {
            printf("Exiting myshell...\n");
            break;
        }

        if (strcmp(args[0], "cd") == 0)
        {
            const char *home = getenv("HOME");
#ifdef _WIN32
            if (home == NULL)
                home = getenv("USERPROFILE");
#endif
            const char *directory = args[1] != NULL ? args[1] : home;

            if (directory == NULL || chdir(directory) != 0)
                perror("cd");

            continue;
        }

        if (strcmp(args[0], "clear") == 0)
        {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            continue;
        }

#ifdef _WIN32
        if (_spawnvp(_P_WAIT, args[0], (const char *const *)args) == -1)
            perror("Command not found");
#else
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            perror("Command not found");
            exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            if (waitpid(pid, &status, 0) == -1)
                perror("waitpid");
        }
#endif
    }

    return 0;
}