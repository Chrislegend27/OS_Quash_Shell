#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

extern char **environ;
pid_t foreground_pid = -1;  // Global variable to store the PID of the current foreground process

// Signal handler for SIGINT (Ctrl-C)
void sigint_handler(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);  // Send SIGINT to the foreground process
    } else {
        printf("\n> ");
        fflush(stdout);
    }
}

// Signal handler for SIGALRM (Timer for long-running processes)
void sigalrm_handler(int sig) {
    if (foreground_pid > 0) {
        printf("\nProcess exceeded time limit (10 seconds). Terminating...\n");
        kill(foreground_pid, SIGKILL);
    }
}

int main() {
    // Register the signal handlers
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigalrm_handler);

    char command_line[MAX_COMMAND_LINE_LEN];
    char *arguments[MAX_COMMAND_LINE_ARGS];
    char cwd[1024];

    while (true) {
        // Print the shell prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd() error");
            printf("> ");
        }
        fflush(stdout);

        // Read input from stdin
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "fgets error");
            exit(EXIT_FAILURE);
        }

        // Handle EOF (Ctrl+D)
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            exit(EXIT_SUCCESS);
        }

        // Skip empty lines
        if (command_line[0] == '\n') {
            continue;
        }

        // Tokenize the input
        int argc = 0;
        char *token = strtok(command_line, " \t\r\n");
        char *input_file = NULL;
        char *output_file = NULL;
        while (token != NULL) {
            if (strcmp(token, ">") == 0) {
                token = strtok(NULL, " \t\r\n");
                output_file = token;
                break;
            } else if (strcmp(token, "<") == 0) {
                token = strtok(NULL, " \t\r\n");
                input_file = token;
                continue;
            }
            arguments[argc++] = token;
            token = strtok(NULL, " \t\r\n");
        }
        arguments[argc] = NULL; // Null-terminate the arguments list

        // Execute built-in commands
        if (strcmp(arguments[0], "cd") == 0) {
            if (arguments[1] == NULL) {
                fprintf(stderr, "cd: expected argument to 'cd'\n");
            } else {
                if (chdir(arguments[1]) != 0) {
                    perror("cd failed");
                }
            }
            continue;
        }

        // External command execution
        pid_t pid = fork();
        if (pid == 0) { // Child process
            if (input_file != NULL) {
                int fd = open(input_file, O_RDONLY);
                if (fd == -1) {
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (output_file != NULL) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(arguments[0], arguments);
            perror("Command execution failed");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork failed");
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            printf("[Process completed] PID: %d\n", pid);
        }
    }
    return 0;
}