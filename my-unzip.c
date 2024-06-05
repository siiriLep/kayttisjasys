#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void myUnzip(char *fileName) {
    FILE *file = fopen(fileName, "rb");
    int charAmount;
    char character;
         
    // If opening the file fails, exit with status code 1
    if (file == NULL) {
        printf("my-unzip: cannot open file\n");
        exit(1);
    }

    // read 5 byte entries from a file
    while (fread(&charAmount, sizeof(int), 1, file) == 1) {  // (size of int = 4 bytes)
        fread(&character, sizeof(char), 1, file); // (size of char = 1 byte)
        for (int i = 0; i < charAmount; i++) {
            printf("%c", character);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    // If no files are specified, exit with status code 1
    if (argc == 1) {
        printf("my-unzip: file1 [file2 ...]\n");
        exit(1);
    }
    // Goes through the given files
    for (int i = 1; i < argc; i++) {
        myUnzip(argv[i]);
    }
    exit(0);
}

// Helped with using fread: https://en.cppreference.com/w/c/io/fread
