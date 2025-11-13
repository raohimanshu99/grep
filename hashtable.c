#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

HashTable* createHashTable() {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < HASH_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
    return ht;
}

void addFileToWord(HashTable* ht, const char* word, int fileIndex, int lineNumber) {
    unsigned int idx = hash(word);
    HashEntry* entry = ht->buckets[idx];
    
    // Search for existing word
    while (entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            // Search for existing file
            FileNode* fn = entry->files;
            while (fn != NULL) {
                if (fn->fileIndex == fileIndex) {
                    // Check if line already exists
                    LineNode* ln = fn->lines;
                    while (ln != NULL) {
                        if (ln->lineNumber == lineNumber) return;
                        ln = ln->next;
                    }
                    // Add new line
                    LineNode* newLine = (LineNode*)malloc(sizeof(LineNode));
                    newLine->lineNumber = lineNumber;
                    newLine->next = fn->lines;
                    fn->lines = newLine;
                    return;
                }
                fn = fn->next;
            }
            // Add new file
            FileNode* newFile = (FileNode*)malloc(sizeof(FileNode));
            newFile->fileIndex = fileIndex;
            newFile->lines = (LineNode*)malloc(sizeof(LineNode));
            newFile->lines->lineNumber = lineNumber;
            newFile->lines->next = NULL;
            newFile->next = entry->files;
            entry->files = newFile;
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry
    HashEntry* newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    newEntry->word = strdup(word);
    newEntry->files = (FileNode*)malloc(sizeof(FileNode));
    newEntry->files->fileIndex = fileIndex;
    newEntry->files->lines = (LineNode*)malloc(sizeof(LineNode));
    newEntry->files->lines->lineNumber = lineNumber;
    newEntry->files->lines->next = NULL;
    newEntry->files->next = NULL;
    newEntry->next = ht->buckets[idx];
    ht->buckets[idx] = newEntry;
}

FileNode* getFilesForWord(HashTable* ht, const char* word) {
    unsigned int idx = hash(word);
    HashEntry* entry = ht->buckets[idx];
    
    while (entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            return entry->files;
        }
        entry = entry->next;
    }
    return NULL;
}

void freeHashTable(HashTable* ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* entry = ht->buckets[i];
        while (entry != NULL) {
            HashEntry* temp = entry;
            FileNode* fn = entry->files;
            while (fn != NULL) {
                FileNode* tempFn = fn;
                LineNode* ln = fn->lines;
                while (ln != NULL) {
                    LineNode* tempLn = ln;
                    ln = ln->next;
                    free(tempLn);
                }
                fn = fn->next;
                free(tempFn);
            }
            entry = entry->next;
            free(temp->word);
            free(temp);
        }
    }
    free(ht);
}
