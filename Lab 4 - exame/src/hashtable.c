#include "hashtable.h"

HashEntry *hashTable[TABLE_SIZE];

unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key;
        key++;
    }
    return hash % TABLE_SIZE;
}

void hashtable_set(const char *key, int value) {
    unsigned int index = hash(key);
    HashEntry *entry = hashTable[index];

    if (entry == NULL) {
        entry = (HashEntry *)malloc(sizeof(HashEntry));
        entry->key = strdup(key);
        entry->value = value;
        hashTable[index] = entry;
    } else {
        free(entry->key);
        entry->key = strdup(key);
        entry->value = value;
    }
}

int hashtable_get(const char *key) {
    unsigned int index = hash(key);
    HashEntry *entry = hashTable[index];

    if (entry != NULL && strcmp(entry->key, key) == 0) {
        return entry->value;
    }

    fprintf(stderr, "Chave não encontrada: %s\n", key);
    exit(EXIT_FAILURE);
}

void freeHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            free(hashTable[i]->key);
            free(hashTable[i]);
        }
    }
}