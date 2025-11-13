#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "hashtable.h"

#define MAX_FILES 1000
#define MAX_FILENAME 256
#define MAX_LINE_LENGTH 1024

extern char* fileList[MAX_FILES];
extern int fileCount;

void readFiles(const char* dirPath, HashTable* ht);
void freeFileList();
int isTextFile(const char* filename);
char* getLineFromFile(const char* filepath, int lineNumber);

#endif
