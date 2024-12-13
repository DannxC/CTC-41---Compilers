#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct {
    char *key;
    int value;
} HashEntry;

extern HashEntry *hashTable[TABLE_SIZE];

unsigned int hash(const char *key);
void hashtable_set(const char *key, int value);
int hashtable_get(const char *key);
void freeHashTable();

#endif // HASHTABLE_H
