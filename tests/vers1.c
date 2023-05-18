#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_ARGS 10
#define MAX_ARG_LEN 100
#define MAX_PATH_LEN 100

char *path[MAX_ARGS];
char *args[MAX_ARGS];
char *envp[MAX_ARGS];
char *line = NULL;
size_t line_size = 0;

void parse_args(char *input, int *argc) {
    char *token = strtok(input, " ");
    int i = 0;
    while (token != NULL && i < MAX_ARGS) {
        args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
    *argc = i;
}

void parse_envp(char **envp) {
    int i = 0;
    while (envp[i] != NULL && i < MAX_ARGS) {
        envp[i] = strdup(envp[i]);
        i++;
    }
    envp[i] = NULL;
}

void parse_path(char *path_str) {
    char *token = strtok(path_str, ":");
    int i = 0;
    while (token != NULL && i < MAX_ARGS) {
        path[i] = token;
        token = strtok(NULL, ":");
        i++;
    }
    path[i] = NULL;
}

char *find_executable(char *cmd) {
    char *executable = malloc(MAX_PATH_LEN);
    struct stat sb;
    int i = 0;
    while (path[i] != NULL) {
        snprintf(executable, MAX_PATH_LEN, "%s/%s", path[i], cmd);
        if (stat(executable, &sb) == 0 && sb.st_mode & S_IXUSR) {
            return executable;
        }
        i++;
    }
    free(executable);
    return NULL;
}

void execute(char *cmd) {
    int argc;
    parse_args(cmd, &argc);
    if (argc == 0) {
        return;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(args[0], "env") == 0) {
        int i = 0;
        while (envp[i] != NULL) {
            printf("%s\n", envp[i]);
            i++;
        }
        return;
    }
    char *executable = find_executable(args[0]);
    if (executable == NULL) {
        printf("%s: command not found\n", args[0]);
        return;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        execve(executable, args, envp);
        perror("execve");
        _exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    free(executable);
}

int main(int argc, char **argv, char **envp) {
    parse_envp(envp);
    char *path_str = getenv("PATH");
    parse_path(path_str);
    while (1) {
        printf("$ ");
        fflush(stdout);
        ssize_t n = getline(&line, &line_size, stdin);
        if (n == -1) {
            if (feof(stdin)) {
                exit(0);
            } else {
                perror("getline");
                exit(1);
            }
        }
        line[strcspn(line, "\n")] = '\0';
        execute(line);
    }
    return 0;
}
