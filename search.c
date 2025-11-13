#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "search.h"
#include "filemanager.h"
#include "wordparser.h"

typedef struct {
    int fileIndex;
    int matchCount;
    LineNode** matchingLines;
    int lineCount;
} SearchResult;

void searchFiles(HashTable* ht, const char* query) {
    char queryWords[10][MAX_WORD];
    int queryWordCount = 0;
    
    // Parse query
    char buffer[256];
    strncpy(buffer, query, 255);
    buffer[255] = '\0';
    
    int wordIndex = 0;
    for (int i = 0; buffer[i] != '\0' && queryWordCount < 10; i++) {
        if (isalnum(buffer[i])) {
            if (wordIndex < MAX_WORD - 1) {
                queryWords[queryWordCount][wordIndex++] = tolower(buffer[i]);
            }
        } else if (wordIndex > 0) {
            queryWords[queryWordCount][wordIndex] = '\0';
            queryWordCount++;
            wordIndex = 0;
        }
    }
    
    if (wordIndex > 0) {
        queryWords[queryWordCount][wordIndex] = '\0';
        queryWordCount++;
    }
    
    if (queryWordCount == 0) {
        printf("No valid search terms entered.\n\n");
        return;
    }
    
    // Collect results
    SearchResult results[MAX_FILES];
    for (int i = 0; i < fileCount; i++) {
        results[i].fileIndex = i;
        results[i].matchCount = 0;
        results[i].matchingLines = malloc(sizeof(LineNode*) * queryWordCount);
        results[i].lineCount = 0;
    }
    
    for (int i = 0; i < queryWordCount; i++) {
        FileNode* files = getFilesForWord(ht, queryWords[i]);
        FileNode* current = files;
        
        while (current != NULL) {
            int idx = current->fileIndex;
            results[idx].matchCount++;
            results[idx].matchingLines[results[idx].lineCount++] = current->lines;
            current = current->next;
        }
    }
    
    // Display results
    printf("\nSearch results for: \"%s\"\n", query);
    printf("=====================================\n");
    
    int found = 0;
    for (int matches = queryWordCount; matches > 0; matches--) {
        for (int i = 0; i < fileCount; i++) {
            if (results[i].matchCount == matches) {
                printf("\n[%d/%d matches] %s\n", matches, queryWordCount, fileList[i]);
                
                // Collect unique line numbers
                int uniqueLines[1000];
                int uniqueCount = 0;
                
                for (int j = 0; j < results[i].lineCount; j++) {
                    LineNode* ln = results[i].matchingLines[j];
                    while (ln != NULL) {
                        int lineNum = ln->lineNumber;
                        int alreadyAdded = 0;
                        for (int k = 0; k < uniqueCount; k++) {
                            if (uniqueLines[k] == lineNum) {
                                alreadyAdded = 1;
                                break;
                            }
                        }
                        if (!alreadyAdded && uniqueCount < 1000) {
                            uniqueLines[uniqueCount++] = lineNum;
                        }
                        ln = ln->next;
                    }
                }
                
                // Sort line numbers
                for (int a = 0; a < uniqueCount - 1; a++) {
                    for (int b = a + 1; b < uniqueCount; b++) {
                        if (uniqueLines[a] > uniqueLines[b]) {
                            int temp = uniqueLines[a];
                            uniqueLines[a] = uniqueLines[b];
                            uniqueLines[b] = temp;
                        }
                    }
                }
                
                // Display lines (limit to first 5)
                int displayCount = uniqueCount < 5 ? uniqueCount : 5;
                for (int j = 0; j < displayCount; j++) {
                    char filepath[512];
                    snprintf(filepath, sizeof(filepath), "./%s", fileList[i]);
                    char* line = getLineFromFile(filepath, uniqueLines[j]);
                    if (line) {
                        printf("  Line %d: ", uniqueLines[j]);
                        for (int k = 0; k < queryWordCount; k++) {
                            highlightWord(line, queryWords[k]);
                            line = getLineFromFile(filepath, uniqueLines[j]);
                        }
                        printf("%s\n", line);
                    }
                }
                if (uniqueCount > 5) {
                    printf("  ... and %d more lines\n", uniqueCount - 5);
                }
                
                found = 1;
            }
        }
    }
    
    if (!found) {
        printf("No files found matching the query.\n");
    }
    
    // Cleanup
    for (int i = 0; i < fileCount; i++) {
        free(results[i].matchingLines);
    }
    
    printf("\n");
}