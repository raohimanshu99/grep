#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "filemanager.h"
#include "wordparser.h"

char* fileList[MAX_FILES];     
char* fileDisplayList[MAX_FILES]; 
int fileCount = 0;

int isTextFile(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return 0;
    const char* textExts[] = {".txt", ".c", ".h", ".cpp", ".java", ".py",
                               ".html", ".css", ".js", ".md", ".log", NULL};
    for (int i = 0; textExts[i] != NULL; i++)
        if (strcasecmp(ext, textExts[i]) == 0) return 1;
    return 0;
}

void indexFileContent(const char* filepath, int fileIndex, HashTable* ht) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("Warning: Could not open %s\n", filepath);
        return;
    }
    char word[MAX_WORD];
    int wordIndex = 0, lineNumber = 1, c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            if (wordIndex > 0) {
                word[wordIndex] = '\0';
                if (wordIndex >= 2) addFileToWord(ht, word, fileIndex, lineNumber);
                wordIndex = 0;
            }
            lineNumber++;
        } else if (isalnum(c)) {
            if (wordIndex < MAX_WORD - 1) word[wordIndex++] = tolower(c);
        } else if (wordIndex > 0) {
            word[wordIndex] = '\0';
            if (wordIndex >= 2) addFileToWord(ht, word, fileIndex, lineNumber);
            wordIndex = 0;
        }
    }
    if (wordIndex > 0) {
        word[wordIndex] = '\0';
        if (wordIndex >= 2) addFileToWord(ht, word, fileIndex, lineNumber);
    }
    fclose(file);
}

char* getLineFromFile(const char* filepath, int lineNumber) {
    FILE* file = fopen(filepath, "r");
    if (!file) return NULL;
    char buffer[MAX_LINE_LENGTH];
    int currentLine = 1;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (currentLine == lineNumber) {
            fclose(file);
            buffer[strcspn(buffer, "\n")] = 0;
            return strdup(buffer);
        }
        currentLine++;
    }
    fclose(file);
    return NULL;
}

static void readFilesRecursive(const char* dirPath, HashTable* ht,
                                const char* rootPath) {
    DIR* dir = opendir(dirPath);
    if (!dir) { printf("Could not open directory: %s\n", dirPath); return; }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && fileCount < MAX_FILES) {
        if (entry->d_name[0] == '.') continue;

        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat st;
        if (stat(fullPath, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            readFilesRecursive(fullPath, ht, rootPath);

        } else if (S_ISREG(st.st_mode) && isTextFile(entry->d_name)) {
            char displayPath[512];
            int rootLen = strlen(rootPath);
            if (strncmp(fullPath, rootPath, rootLen) == 0)
                snprintf(displayPath, sizeof(displayPath), "%s",
                         fullPath + rootLen + (fullPath[rootLen] == '/' ? 1 : 0));
            else
                snprintf(displayPath, sizeof(displayPath), "%s", fullPath);

            fileList[fileCount]        = strdup(fullPath);
            fileDisplayList[fileCount] = strdup(displayPath);

            printf("Indexing: %s\n", displayPath);
            indexFileContent(fullPath, fileCount, ht);
            fileCount++;
        }
    }
    closedir(dir);
}

void readFiles(const char* dirPath, HashTable* ht) {
    char resolvedRoot[512];
    snprintf(resolvedRoot, sizeof(resolvedRoot), "%s", dirPath);
    int len = strlen(resolvedRoot);
    if (len > 1 && resolvedRoot[len - 1] == '/') resolvedRoot[len - 1] = '\0';

    readFilesRecursive(resolvedRoot, ht, resolvedRoot);
    printf("\nIndexed %d text files.\n\n", fileCount);
}

void freeFileList() {
    for (int i = 0; i < fileCount; i++) {
        free(fileList[i]);
        free(fileDisplayList[i]);
    }
}