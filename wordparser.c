#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "wordparser.h"

void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void highlightWord(const char* line, const char* word) {
    int lineLen = strlen(line);
    int wordLen = strlen(word);
    char lowercaseWord[100];

    strncpy(lowercaseWord, word, sizeof(lowercaseWord) - 1);
    lowercaseWord[sizeof(lowercaseWord) - 1] = '\0';
    toLowerCase(lowercaseWord);

    int i = 0;
    while (i < lineLen) {
        int j = 0;
        int matchStart = i;

        while (i < lineLen && j < wordLen && tolower(line[i]) == lowercaseWord[j]) {
            i++;
            j++;
        }

        if (j == wordLen &&
            (matchStart == 0 || !isalnum(line[matchStart - 1])) &&
            (i >= lineLen || !isalnum(line[i]))) {
            printf("\033[1;33m");
            for (int k = matchStart; k < i; k++) {
                printf("%c", line[k]);
            }
            printf("\033[0m");
        } else {
            printf("%c", line[matchStart]);
            i = matchStart + 1;
        }
    }
}