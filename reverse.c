#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

/*************************************
Internet sources used to make this project:
    * Uolevi Nikula's C-Ohjelmointiopas for the linked list creation
    * https://www.youtube.com/watch?v=K7J3nCeRC80 at  around 6:30min to print the linked list in reverse order
*************************************/

// Define a node for the linked list
typedef struct node {
    char line[MAX];
    struct node *pNext;
} NODE; 

NODE* linkedList(char *filename);
NODE* linkedListInput();
void reverseToStdout(NODE *node);
void reverseToOutputFile(NODE *node, FILE *outFile);
void freeMemory(NODE *ptr);


// Linked list creation when file is specified   
NODE* linkedList(char *filename) {
    // Open the specified file
    FILE *file = fopen(filename, "r");
    // Handle error when file is not found
    if (file == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        exit(1);
    }

    char line[MAX];
    NODE *pStart = NULL, *pEnd = NULL;
    NODE *pNew;

    // Linked list creation
    while (fgets(line, MAX, file) != NULL) { // Reads a line from the file and stores it to 'line'
        if ((pNew = (NODE*)malloc(sizeof(NODE))) == NULL) {
            fprintf(stderr, "malloc failed");
            exit(1);
        }
        // copy the line to the node
        strncpy(pNew->line, line, MAX);
        pNew->pNext = NULL;
        // When the list is empty, create the first node
        if (pStart == NULL) {
            pStart = pNew;
            pEnd = pNew;
        } else {    // if not empty, add to the end
            pEnd->pNext = pNew;
            pEnd = pNew;
        }
    }
    fclose(file);
    return pStart;
}

// Linked list when reading stdin
NODE* linkedListInput() {
    NODE *pStart = NULL, *pEnd = NULL;
    NODE *pNew;
    char line[MAX];

    printf("Please enter the lines you want reversed.\n");
    while (fgets(line, MAX, stdin) != NULL && strcmp(line, "\n")) { 
        if ((pNew = (NODE*)malloc(sizeof(NODE))) == NULL) {
            fprintf(stderr, "memory allocation failed");
            exit(1);
        }
        // copy the line to the node
        strncpy(pNew->line, line, MAX);
        pNew->pNext = NULL;
        // When the list is empty, create the first node
        if (pStart == NULL) {
            pStart = pNew;
            pEnd = pNew;
        } else { // if not empty, add to the end
            pEnd->pNext = pNew;
            pEnd = pNew;
        }
    }
    return pStart;
}

// Function to reverse the input to stdout
void reverseToStdout(NODE *node) {
    if (node == NULL) {
        return;
    }
    reverseToStdout(node->pNext);
    printf("%s", node->line);
}

// Function to reverse the input to a specified output file
void reverseToOutputFile(NODE *node, FILE *outFile) {
    if (node == NULL) {
        return;
    }
    reverseToOutputFile(node->pNext, outFile);
    fprintf(outFile, "%s", node->line);
}

void freeMemory(NODE *ptr) {
    NODE *pStart;
    while (ptr != NULL) {
        pStart = ptr;
        ptr = ptr->pNext;
        free(pStart);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        NODE *pStart = linkedListInput();
        reverseToStdout(pStart);
        freeMemory(pStart);
    }
    if (argc == 2) {
        NODE *pStart = linkedList(argv[1]);
        reverseToStdout(pStart);
        freeMemory(pStart);
    } 
    if (argc == 3) {
        if(strcmp(argv[1],argv[2]) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }
        FILE *outFile = fopen(argv[2], "w");
        if (outFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
        NODE *pStart = linkedList(argv[1]);
        reverseToOutputFile(pStart, outFile);
        fclose(outFile);
        freeMemory(pStart);
    } 
    if(argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
    exit(0);
}
