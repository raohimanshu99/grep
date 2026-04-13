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
    LineNode **matchingLines;
    int lineCount;
} SearchResult;

typedef enum { MODE_DEFAULT, MODE_AND, MODE_OR, MODE_NOT } QueryMode;

static QueryMode detectMode(const char* query) {
    char buf[256];
    strncpy(buf, query, 255);
    buf[255] = '\0';

    char* token = strtok(buf, " ");
    while (token != NULL) {
        if (strcmp(token, "AND") == 0) return MODE_AND;
        if (strcmp(token, "OR")  == 0) return MODE_OR;
        if (strcmp(token, "NOT") == 0) return MODE_NOT;
        token = strtok(NULL, " ");
    }
    return MODE_DEFAULT;
}

static void parseQuery(const char* query,
                       char includeWords[][MAX_WORD], int* includeCount,
                       char excludeWords[][MAX_WORD], int* excludeCount) {
    *includeCount = 0;
    *excludeCount = 0;

    char buf[256];
    strncpy(buf, query, 255);
    buf[255] = '\0';

    int notMode = 0; 

    char* token = strtok(buf, " ");
    while (token != NULL) {
        if (strcmp(token, "AND") == 0 || strcmp(token, "OR") == 0) {
            token = strtok(NULL, " ");
            continue;
        }
        if (strcmp(token, "NOT") == 0) {
            notMode = 1;
            token = strtok(NULL, " ");
            continue;
        }

        char lower[MAX_WORD];
        int j = 0;
        for (int i = 0; token[i] && j < MAX_WORD - 1; i++) {
            if (isalnum(token[i])) lower[j++] = tolower(token[i]);
        }
        lower[j] = '\0';
        if (j == 0) { token = strtok(NULL, " "); continue; }

        if (notMode) {
            if (*excludeCount < 10)
                strncpy(excludeWords[(*excludeCount)++], lower, MAX_WORD);
        } else {
            if (*includeCount < 10)
                strncpy(includeWords[(*includeCount)++], lower, MAX_WORD);
        }
        token = strtok(NULL, " ");
    }
}

void searchFiles(HashTable *ht, const char *query) {

    QueryMode mode = detectMode(query);

    char includeWords[10][MAX_WORD];
    char excludeWords[10][MAX_WORD];
    int  includeCount = 0, excludeCount = 0;

    if (mode == MODE_DEFAULT) {
        char buf[256];
        strncpy(buf, query, 255);
        buf[255] = '\0';
        int wordIndex = 0;
        for (int i = 0; buf[i] != '\0' && includeCount < 10; i++) {
            if (isalnum(buf[i])) {
                if (wordIndex < MAX_WORD - 1)
                    includeWords[includeCount][wordIndex++] = tolower(buf[i]);
            } else if (wordIndex > 0) {
                includeWords[includeCount][wordIndex] = '\0';
                includeCount++;
                wordIndex = 0;
            }
        }
        if (wordIndex > 0) {
            includeWords[includeCount][wordIndex] = '\0';
            includeCount++;
        }
    } else {
        parseQuery(query, includeWords, &includeCount, excludeWords, &excludeCount);
    }

    if (includeCount == 0) {
        printf("No valid search terms entered.\n\n");
        return;
    }

    SearchResult *results = malloc(sizeof(SearchResult) * fileCount);
    if (!results) { printf("Memory allocation failed.\n"); return; }

    for (int i = 0; i < fileCount; i++) {
        results[i].fileIndex   = i;
        results[i].matchCount  = 0;
        results[i].matchingLines = malloc(sizeof(LineNode *) * includeCount);
        results[i].lineCount   = 0;
    }

    for (int i = 0; i < includeCount; i++) {
        FileNode *current = getFilesForWord(ht, includeWords[i]);
        while (current != NULL) {
            int idx = current->fileIndex;
            results[idx].matchCount++;
            results[idx].matchingLines[results[idx].lineCount++] = current->lines;
            current = current->next;
        }
    }

    if (excludeCount > 0) {
        for (int e = 0; e < excludeCount; e++) {
            FileNode *current = getFilesForWord(ht, excludeWords[e]);
            while (current != NULL) {
                results[current->fileIndex].matchCount = -1; 
                current = current->next;
            }
        }
    }

    printf("\nSearch results for: \"%s\"\n", query);
    if (mode == MODE_AND)     printf("Mode: AND — showing files with ALL keywords\n");
    else if (mode == MODE_OR) printf("Mode: OR  — showing files with ANY keyword\n");
    else if (mode == MODE_NOT)printf("Mode: NOT — excluding files with: ");
    if (mode == MODE_NOT) {
        for (int e = 0; e < excludeCount; e++)
            printf("%s%s", excludeWords[e], e < excludeCount-1 ? ", " : "");
        printf("\n");
    }
    printf("=====================================\n");

    int found = 0;

    if (mode == MODE_AND) {
        for (int i = 0; i < fileCount; i++) {
            if (results[i].matchCount != includeCount) continue;
            found = 1;
            printf("\n[ALL %d matched] %s\n", includeCount, fileDisplayList[i]);
            int uniqueLines[1000], uniqueCount = 0;
            for (int j = 0; j < results[i].lineCount; j++) {
                LineNode *ln = results[i].matchingLines[j];
                while (ln != NULL) {
                    int dup = 0;
                    for (int k = 0; k < uniqueCount; k++)
                        if (uniqueLines[k] == ln->lineNumber) { dup=1; break; }
                    if (!dup && uniqueCount < 1000) uniqueLines[uniqueCount++] = ln->lineNumber;
                    ln = ln->next;
                }
            }
            for (int a = 0; a < uniqueCount-1; a++)
                for (int b = a+1; b < uniqueCount; b++)
                    if (uniqueLines[a] > uniqueLines[b]) {
                        int t = uniqueLines[a]; uniqueLines[a]=uniqueLines[b]; uniqueLines[b]=t;
                    }
            int show = uniqueCount < 5 ? uniqueCount : 5;
            for (int j = 0; j < show; j++) {
                char *line = getLineFromFile(fileList[i], uniqueLines[j]);
                if (line) {
                    printf("  Line %d: ", uniqueLines[j]);
                    for (int pos = 0; line[pos];) {
                        int matched = 0;
                        for (int k = 0; k < includeCount; k++) {
                            int wlen = strlen(includeWords[k]), m = 0;
                            while (m < wlen && tolower(line[pos+m]) == includeWords[k][m]) m++;
                            if (m == wlen &&
                                (pos==0 || !isalnum(line[pos-1])) &&
                                !isalnum(line[pos+wlen])) {
                                printf("\033[1;33m");
                                for (int x=0; x<wlen; x++) printf("%c", line[pos+x]);
                                printf("\033[0m");
                                pos += wlen; matched = 1; break;
                            }
                        }
                        if (!matched) { printf("%c", line[pos]); pos++; }
                    }
                    printf("\n");
                    free(line);
                }
            }
            if (uniqueCount > 5) printf("  ... and %d more lines\n", uniqueCount - 5);
        }

    } else if (mode == MODE_OR) {
        for (int matches = includeCount; matches > 0; matches--) {
            for (int i = 0; i < fileCount; i++) {
                if (results[i].matchCount != matches) continue;
                found = 1;
                printf("\n[%d/%d matched] %s\n", matches, includeCount, fileDisplayList[i]);
                int uniqueLines[1000], uniqueCount = 0;
                for (int j = 0; j < results[i].lineCount; j++) {
                    LineNode *ln = results[i].matchingLines[j];
                    while (ln != NULL) {
                        int dup = 0;
                        for (int k = 0; k < uniqueCount; k++)
                            if (uniqueLines[k] == ln->lineNumber) { dup=1; break; }
                        if (!dup && uniqueCount < 1000) uniqueLines[uniqueCount++] = ln->lineNumber;
                        ln = ln->next;
                    }
                }
                for (int a = 0; a < uniqueCount-1; a++)
                    for (int b = a+1; b < uniqueCount; b++)
                        if (uniqueLines[a] > uniqueLines[b]) {
                            int t = uniqueLines[a]; uniqueLines[a]=uniqueLines[b]; uniqueLines[b]=t;
                        }
                int show = uniqueCount < 5 ? uniqueCount : 5;
                for (int j = 0; j < show; j++) {
                    char *line = getLineFromFile(fileList[i], uniqueLines[j]);
                    if (line) {
                        printf("  Line %d: ", uniqueLines[j]);
                        for (int pos = 0; line[pos];) {
                            int matched = 0;
                            for (int k = 0; k < includeCount; k++) {
                                int wlen = strlen(includeWords[k]), m = 0;
                                while (m < wlen && tolower(line[pos+m]) == includeWords[k][m]) m++;
                                if (m == wlen &&
                                    (pos==0 || !isalnum(line[pos-1])) &&
                                    !isalnum(line[pos+wlen])) {
                                    printf("\033[1;33m");
                                    for (int x=0; x<wlen; x++) printf("%c", line[pos+x]);
                                    printf("\033[0m");
                                    pos += wlen; matched = 1; break;
                                }
                            }
                            if (!matched) { printf("%c", line[pos]); pos++; }
                        }
                        printf("\n");
                        free(line);
                    }
                }
                if (uniqueCount > 5) printf("  ... and %d more lines\n", uniqueCount - 5);
            }
        }

    } else {
        for (int matches = includeCount; matches > 0; matches--) {
            for (int i = 0; i < fileCount; i++) {
                if (results[i].matchCount != matches) continue;
                found = 1;
                printf("\n[%d/%d matches] %s\n", matches, includeCount, fileDisplayList[i]);
                int uniqueLines[1000], uniqueCount = 0;
                for (int j = 0; j < results[i].lineCount; j++) {
                    LineNode *ln = results[i].matchingLines[j];
                    while (ln != NULL) {
                        int dup = 0;
                        for (int k = 0; k < uniqueCount; k++)
                            if (uniqueLines[k] == ln->lineNumber) { dup=1; break; }
                        if (!dup && uniqueCount < 1000) uniqueLines[uniqueCount++] = ln->lineNumber;
                        ln = ln->next;
                    }
                }
                for (int a = 0; a < uniqueCount-1; a++)
                    for (int b = a+1; b < uniqueCount; b++)
                        if (uniqueLines[a] > uniqueLines[b]) {
                            int t = uniqueLines[a]; uniqueLines[a]=uniqueLines[b]; uniqueLines[b]=t;
                        }
                int show = uniqueCount < 5 ? uniqueCount : 5;
                for (int j = 0; j < show; j++) {
                    char *line = getLineFromFile(fileList[i], uniqueLines[j]);
                    if (line) {
                        printf("  Line %d: ", uniqueLines[j]);
                        for (int pos = 0; line[pos];) {
                            int matched = 0;
                            for (int k = 0; k < includeCount; k++) {
                                int wlen = strlen(includeWords[k]), m = 0;
                                while (m < wlen && tolower(line[pos+m]) == includeWords[k][m]) m++;
                                if (m == wlen &&
                                    (pos==0 || !isalnum(line[pos-1])) &&
                                    !isalnum(line[pos+wlen])) {
                                    printf("\033[1;33m");
                                    for (int x=0; x<wlen; x++) printf("%c", line[pos+x]);
                                    printf("\033[0m");
                                    pos += wlen; matched = 1; break;
                                }
                            }
                            if (!matched) { printf("%c", line[pos]); pos++; }
                        }
                        printf("\n");
                        free(line);
                    }
                }
                if (uniqueCount > 5) printf("  ... and %d more lines\n", uniqueCount - 5);
            }
        }
    }

    if (!found) printf("No files found matching the query.\n");
    else {
        int totalFiles = 0, totalMatches = 0;
        for (int i = 0; i < fileCount; i++)
            if (results[i].matchCount > 0) { totalFiles++; totalMatches += results[i].matchCount; }
        printf("\n-------------------------------\n");
        printf("✅Found %d match(es) across %d file(s)\n", totalMatches, totalFiles);
        printf("-------------------------------\n");
    }

    for (int i = 0; i < fileCount; i++) free(results[i].matchingLines);
    free(results);
    printf("\n");
}