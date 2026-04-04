#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "filemanager.h"
#include "wordparser.h"

char* fileList[MAX_FILES];
int fileCount = 0;

int isTextFile(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return 0;
    
    const char* textExts[] = {".txt", ".c", ".h", ".cpp", ".java", ".py", 
                               ".html", ".css", ".js", ".md", ".log", NULL};
    
    for (int i = 0; textExts[i] != NULL; i++) {
        if (strcasecmp(ext, textExts[i]) == 0) return 1;
    }
    return 0;
}

void indexFileContent(const char* filepath, int fileIndex, HashTable* ht) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("Warning: Could not open %s\n", filepath);
        return;
    }
    
    char word[MAX_WORD];
    int wordIndex = 0;
    int lineNumber = 1;
    int c;
    
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            if (wordIndex > 0) {
                word[wordIndex] = '\0';
                if (wordIndex >= 2) {
                    addFileToWord(ht, word, fileIndex, lineNumber);
                }
                wordIndex = 0;
            }
            lineNumber++;
        } else if (isalnum(c)) {
            if (wordIndex < MAX_WORD - 1) {
                word[wordIndex++] = tolower(c);
            }
        } else if (wordIndex > 0) {
            word[wordIndex] = '\0';
            if (wordIndex >= 2) {
                addFileToWord(ht, word, fileIndex, lineNumber);
            }
            wordIndex = 0;
        }
    }
    
    // Add last word if exists
    if (wordIndex > 0) {
        word[wordIndex] = '\0';
        if (wordIndex >= 2) {
            addFileToWord(ht, word, fileIndex, lineNumber);
        }
    }
    
    fclose(file);
}

char* getLineFromFile(const char* filepath, int lineNumber) {
    FILE* file = fopen(filepath, "r");
    if (!file) return NULL;
    
    static char line[MAX_LINE_LENGTH];
    int currentLine = 1;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        if (currentLine == lineNumber) {
            fclose(file);
            // Remove trailing newline
            line[strcspn(line, "\n")] = 0;
            return line;
        }
        currentLine++;
    }
    
    fclose(file);
    return NULL;
}

void readFiles(const char* dirPath, HashTable* ht) {
    DIR* dir = opendir(dirPath);
    if (dir == NULL) {
        printf("Could not open directory: %s\n", dirPath);
        return;
    }
    
    struct dirent* entry;
    int indexed = 0;
    
    while ((entry = readdir(dir)) != NULL && fileCount < MAX_FILES) {
        if (entry->d_name[0] == '.') continue;
        
        if (!isTextFile(entry->d_name)) continue;
        
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirPath, entry->d_name);
        
        struct stat st;
        if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode)) {
            fileList[fileCount] = strdup(entry->d_name);
            
            printf("Indexing: %s\n", entry->d_name);
            indexFileContent(filepath, fileCount, ht);
            
            fileCount++;
            indexed++;
        }
    }
    
    closedir(dir);
    printf("\nIndexed %d text files.\n\n", indexed);
}

void freeFileList() {
    for (int i = 0; i < fileCount; i++) {
        free(fileList[i]);
    }
}