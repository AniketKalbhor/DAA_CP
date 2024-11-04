//
// Created by anika on 22-10-2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzw.h"

#define MAX_DICT_SIZE 4096
#define BYTE_SIZE 256

typedef struct {
    int code;
    char sequence[256];
} DictionaryEntry;

DictionaryEntry dictionary[MAX_DICT_SIZE];
int dictSize = BYTE_SIZE;

void initializeDictionary() {
    for (int i = 0; i < BYTE_SIZE; i++) {
        dictionary[i].code = i;
        dictionary[i].sequence[0] = (char)i;
        dictionary[i].sequence[1] = '\0';
    }
}

int findInDictionary(char *sequence) {
    for (int i = 0; i < dictSize; i++) {
        if (strcmp(dictionary[i].sequence, sequence) == 0) {
            return dictionary[i].code;
        }
    }
    return -1;
}

void addToDictionary(char *sequence) {
    if (dictSize < MAX_DICT_SIZE) {
        dictionary[dictSize].code = dictSize;
        strcpy(dictionary[dictSize].sequence, sequence);
        dictSize++;
    }
}

char* lzw_compress(const char* input) {
    initializeDictionary();

    char currentSeq[256] = "";
    char nextChar[2] = "";
    int code;

    // Allocate memory for the output string
    char* output = malloc(strlen(input) * 10); // Allocate more than needed
    char* current = output;

    // Traverse the input string
    for (int i = 0; input[i] != '\0'; i++) {
        nextChar[0] = input[i];
        nextChar[1] = '\0';

        char newSeq[256];
        strcpy(newSeq, currentSeq);
        strcat(newSeq, nextChar);

        if (findInDictionary(newSeq) != -1) {
            strcpy(currentSeq, newSeq);
        } else {
            code = findInDictionary(currentSeq);
            current += sprintf(current, "%d ", code);

            addToDictionary(newSeq);

            strcpy(currentSeq, nextChar);
        }
    }

    if (currentSeq[0] != '\0') {
        code = findInDictionary(currentSeq);
        current += sprintf(current, "%d ", code);
    }

    return output;
}