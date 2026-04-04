#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "filemanager.h"
#include "wordparser.h"
#include "search.h"

int main(int argc, char* argv[]) {
    HashTable* ht = createHashTable();
    
    printf("File Content Search System\n");
    printf("==========================\n\n");

    const char* dirPath = ".";
    if (argc > 1) {
        dirPath = argv[1];
    }

    printf("Searching in: %s\n\n", dirPath);
    printf("Reading and indexing files...\n");
    readFiles(dirPath, ht);
    
    if (fileCount == 0) {
        printf("No text files found to index.\n");
        freeHashTable(ht);
        return 1;
    }
    
    char query[256];
    printf("You can now search for keywords within file contents.\n");
    printf("Supported file types: .txt, .c, .h, .cpp, .java, .py, .html, .css, .js, .md, .log\n\n");
    
    while (1) {
        printf("Enter search query (or 'quit' to exit): ");
        if (fgets(query, sizeof(query), stdin) == NULL) break;
        
        query[strcspn(query, "\n")] = 0;
        
        if (strcmp(query, "quit") == 0) break;
        if (strlen(query) == 0) continue;
        
        searchFiles(ht, query);
    }
    
    freeHashTable(ht);
    freeFileList();
    
    printf("Goodbye!\n");
    return 0;
}