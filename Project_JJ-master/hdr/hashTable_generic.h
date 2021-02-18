#ifndef _HASHGEN_H
#define _HASHGEN_H

typedef struct HashTable_gen HashTable_gen;
//typedef struct HashBucket_gen HashBucket_gen;
typedef struct PairInfo_gen PairInfo_gen;

struct HashTable_gen
{
    long size;
    long itemsInserted;
    HashBucket_gen **hashArray;
};

struct HashBucket_gen
{
    void *bucketList;
};

HashTable_gen *initHashTable_gen(int initItemsN);

void freeHashBucket_gen(HashBucket_gen *hashBucket);

void freeHashTable_gen(HashTable_gen *hashTable);

#endif