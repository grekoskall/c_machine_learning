#include "../hdr/includes.h"

HashTable_gen *initHashTable_gen(int initItemsN)
{
    int bucketsToAlloc = initItemsN / 0.5; // we need a load factor of at least 70%, to minimize collisions
                                           // let's double the size here
    bucketsToAlloc = closestPrime(bucketsToAlloc);

    HashTable_gen *newHashTable = (HashTable_gen *)safe_malloc(sizeof(HashTable_gen));
    newHashTable->size = bucketsToAlloc;
    newHashTable->itemsInserted = 0;
    newHashTable->hashArray = (HashBucket_gen **)safe_calloc(bucketsToAlloc, sizeof(HashBucket_gen *));

    for (int i = 0; i < bucketsToAlloc; i++)
    {
        newHashTable->hashArray[i] = NULL;
    }
    return newHashTable;
}

void freeHashBucket_gen(HashBucket_gen *hashBucket)
{
    if (hashBucket == NULL)
    {
        return;
    }
    free(hashBucket);
}

void freeHashTable_gen(HashTable_gen *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        freeHashBucket_gen(hashTable->hashArray[i]);
    }
    free(hashTable->hashArray);
    free(hashTable);
}