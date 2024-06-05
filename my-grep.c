#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

void myGrep(char *search, char *filename) {
    FILE *file = fopen(filename, "r");
    char line[MAX];

    // If opening the file fails, exit with status code 1
    if (file == NULL) {
        printf("my-grep: cannot open file\n");
        exit(1);
    }
    while (fgets(line, MAX, file) != NULL) {
        //strstr compares strings in the file to the given searchword. If a match is found, show the line to user
        if (strstr(line, search) != NULL) { 
            printf("%s", line);
        }
    }
    fclose(file);
}


void grepFromInput(char *search) {
    char line[MAX];
    char lines[MAX][MAX]; 
    int lineAmount = 0;

    printf("Please enter your text. \n");
    // Get lines given by user and copy them
    while (fgets(line, MAX, stdin) != NULL) {
        if (strcmp(line, "\n") == 0) {
            break;
        }
        strcpy(lines[lineAmount],line); 
        lineAmount++;
    }
    // Goes through lines and checks for the given searchword
    for (int i = 0; i < lineAmount; i++) {
        if (strstr(lines[i], search) != NULL) {
            printf("%s", lines[i]); 
        }
    }
}


int main(int argc, char *argv[]) {
    // Give user instructions if no arguments are passed
    if (argc == 1) {
        printf("my-grep: searchterm [file ...]\n");
        exit(1);
    }
    // If only the searchword is given, read user input
    if (argc == 2)  {
        grepFromInput(argv[1]);
    }
    // Goes through the given files
    for (int i = 2; i < argc; i++) {
        myGrep(argv[1], argv[i]);
    }
    exit(0);
}
