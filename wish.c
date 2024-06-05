#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX 128

void parseInput(char *buffer, char *pathList[MAX]);


// Checks if a command is built in
int isBuiltInCommand(char *command) {
    if (strcmp(command, "cd") == 0 || strcmp(command, "exit") == 0 || strcmp(command, "path") == 0) {
        return 1;
    }
    return 0;
}

void freeMem(char *array[MAX]) {
    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
        array[i] = NULL;
    }
}


// Executes commands that are *not* built-in
int execute(char *argList[], char *programPath) {
    char error_message[30] = "An error has occurred\n";
    pid_t pid = fork();

    if (pid < 0) { // Show an error msg if creating a child process fails
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    } else if (pid == 0) { // new process
        execv(programPath, argList);
        // function only returns if error occurs
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    free(programPath); 
    return pid;
}


// Builds a path for the program, and checks if it is accessible
int checkAccess(char *argList[MAX], char *pathList[MAX]) {
    char error_message[30] = "Program not found\n";
    char *program = argList[0];  // the program, e.g. "ls" is stored as the first argument
    char programPath[MAX];
    int i;

    // Checks paths for access
    for (i = 0; pathList[i] != NULL; i++) {
        // Build the path
        strcpy(programPath, pathList[i]);
        strcat(programPath, "/");
        strcat(programPath, program);
        if (access(programPath, X_OK) == 0) {
            int pid = execute(argList, strdup(programPath));
            return pid;
        }
    }

    // Tell the user and return to loop if program is not found
    write(STDERR_FILENO, error_message, strlen(error_message));
    return -1;
}


// Batch mode which reads input from batch file and executes commands in it
void batch(char *fileName) {
    char *pathList[MAX];
    FILE *file = fopen(fileName, "r");
    char error_message[30] = "An error has occurred\n";
    pathList[0] = strdup("/bin");  // Initial directory
    pathList[1] = NULL;
    char *line = NULL;
    size_t size = 0;

    // If accessing the file fails, exit
    if (file == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    // Gets lines from file
    while ((getline(&line, &size, file)) != -1) {
        parseInput(line, pathList);
    }

    // Close file and free memory
    fclose(file);
    free(line);
    freeMem(pathList);
    return;

}


// Redirection to specified file
void redirection(char *argList[MAX]) {
    char error_message[35] = "Too many files for redirection\n";
    int argcount = 0;
    for (int i = 0; argList[i] != 0; i++) {
        argcount++;
    }

    // Source used: https://stackoverflow.com/questions/29154056/redirect-stdout-to-a-file
    for (int i = 0; i < argcount; i++) {
        if (strcmp(argList[i], ">") == 0) {
            // Check that there is one argument after redirection
            if (i != argcount - 2) {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            } else {
                freopen(argList[i + 1], "w", stdout); // Open the file in "write"-mode
                argList[i] = NULL;
                break;
            }
        }
    }
}

// Built-in commands ("cd", "exit", "path")
void builtInCommands(char *argList[MAX], char *pathList[MAX]) {
    char error_message[30] = "An error has occurred\n";
    int argcount = 0;
    for (int i = 0; argList[i] != 0; i++) {
        argcount++;
    }

    // Check if first argument is exit
    if (strcmp(argList[0], "exit") == 0) {
        // Give error if user passes too many arguments with exit
        if (argcount > 1) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        } else {
            freeMem(pathList);
            freeMem(argList);
            exit(0);
        }
    }
    // Check if first argument is cd
    if (strcmp(argList[0], "cd") == 0) {
        // Wrong amount of arguments is signaled as error
        if (argcount != 2) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        } else {
            // Change to specified directory
            if (chdir(argList[1]) != 0) {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            return;
        }
    }
    // Check if first argument is path
    if (strcmp(argList[0], "path") == 0) {
        // If no paths are specified, remove paths
        if (argcount == 1) {
            freeMem(pathList);
            return;
        // Add new path
        } else {
            int pathIndex = 0;
            int j = 1;
            // Find end of path array
            while (pathList[pathIndex] != NULL) {
                pathIndex++;
            }
            // Add new paths to the end of pathlist
            for (j = 1; j < argcount; j++) {
                pathList[pathIndex] = strdup(argList[j]);
                pathIndex++;
            }
            pathList[pathIndex] = NULL; // Set last to NULL
            return;
        }
    }
}


// Parse input and wait for parallel execution
void parseInput(char *buffer, char *pathList[MAX]) {
    char *token;
    char *command[MAX];
    int i = 0;
    pid_t pidList[MAX];
    int pidAmount = 0;
    const char s[6] = " \n\t"; // Break commands into tokens

    token = strtok(buffer, s);  // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm

    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            command[i] = NULL; // End the command
            // Check if there is redirection
            redirection(command);
            // Check if the command is built in
            if (isBuiltInCommand(command[0])) {
                builtInCommands(command, pathList); // Execute built in command
            } else {
                // If the command is not built in, check access and build the path
                int pid = checkAccess(command, pathList);
                if (pid != -1) { // If program is found, add pid to pidList
                    pidList[pidAmount++] = pid;
                }
            }
            i = 0;
        } else {
            command[i++] = token;
        }
        token = strtok(NULL, s);
    }

    // Get last command
    if (i != 0) {
        command[i] = NULL;
        redirection(command); // Handle redirection
        if (isBuiltInCommand(command[0])) {
            builtInCommands(command, pathList);
        } else {
            int pid = checkAccess(command, pathList);
            if (pid != -1) {
                pidList[pidAmount++] = pid;
            }
        }
    }

    // Wait for all child processes to finish
    for (int j = 0; j < pidAmount; j++) {
        waitpid(pidList[j], NULL, 0);   // https://stackoverflow.com/questions/23204128/waiting-for-all-children-with-waitpid-option-wall
    }

    // Makes sure that output is directed to stdout
    freopen("/dev/tty", "w", stdout);
}

// Interactive mode that lets users type commands directly
void interactive() {
    char *pathList[MAX]; // List to store paths
    pathList[0] = strdup("/bin");
    pathList[1] = NULL;
    char *buffer = NULL;
    size_t bufferSize = 0;
    int feof = 0;

    while(1) {
        printf("wish>");
        feof = getline(&buffer, &bufferSize, stdin);
        if (feof == -1) {
            free(buffer);
            freeMem(pathList);
            exit(0);
        }
        parseInput(buffer, pathList);
    }

    free(buffer);
    freeMem(pathList);
}

int main(int argc, char *argv[]) {
    char error_message[30] = "An error has occurred\n";
    // If no files are specified, start interactive mode
    if (argc == 1) {
        interactive();
        exit(0);
    }
    // If a file is specified, start batch mode
    if (argc == 2) {
        batch(argv[1]);
        exit(0);
    }
    // Too many arguments results in error
    if (argc > 2) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    exit(0);
}
