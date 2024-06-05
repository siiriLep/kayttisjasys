#include <stdio.h> 
#include <stdlib.h>
#define MAX 1024


void myZip(char *filename) {
    FILE *file = fopen(filename, "r");
    char line[MAX];
    char character;

    // If opening the file fails, exit with status code 1
    if (file == NULL) {
        printf("my-zip: cannot open file\n");
        exit(1);
    }

    while (fgets(line, MAX, file) != NULL) {
        int charAmount = 1; // Restart the counter after a line
        // Loop that goes through a line in a file
        for (int i = 0; line[i] != '\0'; i++) {
            character = line[i]; 
            // If next character is the same, add to amount
            if (character == line[i + 1]) {
                charAmount++;
            } else {
                fwrite(&charAmount, sizeof(int), 1, stdout);
                //printf("%d", charAmount);
                charAmount = 1;
                //printf("%c", character);
                fwrite(&character, sizeof(char), 1, stdout);
            }
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    // If no files are specified, exit with status code 1
    if (argc == 1) {
        printf("my-zip: file1 [file2 ...]\n");
        exit(1);
    }
    // Goes through the given files
    for (int i = 1; i < argc; i++) {
        myZip(argv[i]);
    }
    exit(0);
}

