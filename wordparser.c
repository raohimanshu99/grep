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
    char lowercaseLine[1024];
    char lowercaseWord[100];
    
    strncpy(lowercaseLine, line, sizeof(lowercaseLine) - 1);
    lowercaseLine[sizeof(lowercaseLine) - 1] = '\0';
    toLowerCase(lowercaseLine);
    
    strncpy(lowercaseWord, word, sizeof(lowercaseWord) - 1);
    lowercaseWord[sizeof(lowercaseWord) - 1] = '\0';
    toLowerCase(lowercaseWord);
    
    int i = 0;
    while (i < lineLen) {
        int j = 0;
        int matchStart = i;
        
        // Check for word match
        while (i < lineLen && j < wordLen && tolower(line[i]) == lowercaseWord[j]) {
            i++;
            j++;
        }
        
        // Verify it's a complete word match
        if (j == wordLen && 
            (matchStart == 0 || !isalnum(line[matchStart - 1])) &&
            (i >= lineLen || !isalnum(line[i]))) {
            // Print everything before the match
            for (int k = matchStart - (matchStart == 0 ? 0 : 0); k < matchStart; k++) {
                if (k >= 0) printf("%c", line[k]);
            }
            // Print highlighted match
            printf("\033[1;33m"); // Yellow bold
            for (int k = matchStart; k < i; k++) {
                printf("%c", line[k]);
            }
            printf("\033[0m"); // Reset
        } else {
            // No match, print character and continue
            i = matchStart + 1;
            if (matchStart < lineLen) {
                printf("%c", line[matchStart]);
            }
        }
    }
}
