#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void parse_input(char *input, char **args) {
    char *token = strtok(input, " \t\n");
    int i = 0;

    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

int execute_command(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "env") == 0) {
        extern char **environ;
        for (char **env = environ; *env; ++env) {
            printf("%s\n", *env);
        }
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("Error");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

    return 1;
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            if (feof(stdin)) {
                printf("\n");
                exit(0);
            } else {
                perror("Error");
                continue;
            }
        }

        parse_input(input, args);

        if (args[0] != NULL) {
            execute_command(args);
        }
    }

    return 0;
}
