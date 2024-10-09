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

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;



int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
    char cwd[1024];
    	
    while (true) {
      
        do{ 
 
            
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s> ", cwd);  // Print the current working directory followed by '> '
            } else {
                perror("getcwd() error");
                // If getcwd() fails, print a default prompt
                printf("> ");
            }
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        if (command_line[0] == '\n') {
            continue;
        }

        // TODO:
        // 			  
        // Tokenize the command line input
        int i = 0;
        char *token = strtok(command_line, delimiters);
        while (token != NULL) {
            arguments[i++] = token;
            token = strtok(NULL, delimiters);
        }
        arguments[i] = NULL; // NULL-terminate the array of tokens

        // Debugging output to verify tokens
        printf("Tokens:\n");
        int j = 0;
        for (j; j < i; j++) {
            printf("Token %d: %s\n", j, arguments[j]);
        }





        // Additional implementation for built-in commands, child process creation would go here...    
      //pwd command
      if (strcmp(arguments[0], "pwd") == 0) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
          printf("%s\n", cwd); // Print the current working directory
        } else {
        perror("getcwd() error"); // Print an error if getcwd fails
        }
      }
      //exit command 
      if (strcmp(arguments[0], "exit") == 0) {
        exit(0);
      }

      //cd command
      if (strcmp(arguments[0], "cd") == 0) {
        if (arguments[1] == NULL) {
            fprintf(stderr, "cd: expected argument to \"cd\"\n");
        } else {
            if (chdir(arguments[1]) != 0) {
                perror("cd error");
            }
        }
      }

      //Echo command
      if (strcmp(arguments[0], "echo") == 0) {
        int arg_index = 1;
      for (arg_index; arguments[arg_index] != NULL; arg_index++) {
          printf("%s", arguments[arg_index]);
          if (arguments[arg_index + 1] != NULL) {
              printf(" "); // Print a space between arguments, but not after the last one
          }
        }
      printf("\n"); // Print a newline after the last argument
      }

      if (strcmp(arguments[0], "setenv") == 0) {
          if (arguments[1] == NULL || arguments[2] == NULL) {
              fprintf(stderr, "setenv: too few arguments\n");
              fprintf(stderr, "Usage: setenv VAR VALUE\n");
          } else {
              if (setenv(arguments[1], arguments[2], 1) != 0) {
                  perror("setenv error");
              }
          }
      }

      if (strcmp(arguments[0], "env") == 0) {
          char **env = environ;
          while (*env) {
              printf("%s\n", *env);  // Print each environment variable
              env++;
          }
      }















      







  

        
      
        // 2. Implement Built-In Commands
      
    
        // 3. Create a child process which will execute the command line input

  
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}