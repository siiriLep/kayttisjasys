#include <stdio.h>
#include <stdlib.h>
#define MAX 1024

// Prints file contents to stdout
void myCat(char *filename) {
    FILE *file = fopen(filename, "r");
    char line[MAX];

    // If opening the file fails, exit with status code 1
    if (file == NULL) {
        printf("my-cat: cannot open file\n");
        exit(1);
    }
    // Read line from file and print to user
    while (fgets(line, MAX, file) != NULL) {
        printf("%s", line);

    }
    printf("\n"); // Newline after a file for readability
    fclose(file);
}

int main(int argc, char *argv[]) {
    // If no files are specified, exit with status code 0
    if (argc == 1) {
        exit(0);
    }
    // Goes through the given files
    for (int i = 1; i < argc; i++) {
        myCat(argv[i]);
    }
    exit(0);
}