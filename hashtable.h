#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASH_SIZE 10007
#define MAX_WORD 100

typedef struct LineNode {
    int lineNumber;
    struct LineNode* next;
} LineNode;

typedef struct FileNode {
    int fileIndex;
    LineNode* lines;
    struct FileNode* next;
} FileNode;

typedef struct HashEntry {
    char* word;
    FileNode* files;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    HashEntry* buckets[HASH_SIZE];
} HashTable;

HashTable* createHashTable();
void addFileToWord(HashTable* ht, const char* word, int fileIndex, int lineNumber);
FileNode* getFilesForWord(HashTable* ht, const char* word);
void freeHashTable(HashTable* ht);
unsigned int hash(const char* str);

#endif