#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64
#define MAX_PATH_LENGTH 1024

void handle_signal(int signo) {
    printf("\n");
}

char *read_input() {
    char *input = NULL;
    size_t input_size = 0;
    ssize_t input_length = getline(&input, &input_size, stdin);
    if (input_length == -1) {
        free(input);
        input = NULL;
    } else {
        input[input_length - 1] = '\0';
    }
    return input;
}

char **parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char *));
    char *arg = strtok(input, " ");
    int i = 0;
    while (arg != NULL && i < MAX_ARGS - 1) {
        args[i++] = arg;
        arg = strtok(NULL, " ");
    }
    args[i] = NULL;
    return args;
}

void execute_command(char **args) {
    if (args[0] == NULL) {
        return;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        execvp(args[0], args);
        perror(args[0]);
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    signal(SIGINT, handle_signal);
    while (1) {
        printf("$ ");
        fflush(stdout);
        char *input = read_input();
        if (input == NULL) {
            printf("\n");
            exit(0);
        }
        char **args = parse_input(input);
        execute_command(args);
        free(input);
        free(args);
    }
    return 0;
}
