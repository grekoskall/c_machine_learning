#ifndef _HASH_H
#define _HASH_H

#include "../hdr/clique.h"

typedef struct HashTable HashTable;
typedef struct HashBucket HashBucket;
typedef struct SpecNode SpecNode;

struct HashTable
{
    long size;
    HashBucket **hashArray;
};

struct HashBucket
{
    SpecNode *specList;
};

struct SpecNode
{
    CliqueNode *cliquePtr;
    SpecNode *nextSpec;
};

SpecNode *initSpecNode();

HashTable *initHashTable(int);

unsigned long long hashFunction(char *);

void addToHashTable(HashTable *, SpecInfo *);

void insertInChain(HashBucket *bucketDst, SpecNode *newSpecNode);

SpecNode *searchHashTable(HashTable *hashTable, char *specId);

SpecNode *searchChain(SpecNode *head, char *specId);

void printAllMatches(HashTable *hashTable, FILE *fptr);

void printSpecMatchesInChain(SpecNode *head, FILE *fptr);

void printAllMissMatches(HashTable *hashTable, FILE *fptr);

void printSpecMissMatchesInChain(SpecNode *head, FILE *fptr);

void printVisitedSpecNodesCount();

void printHashTable(HashTable *hashTable);

void printSpecChain(SpecNode *head);

void freeSpecNode(SpecNode *specNode);

void freeHashBucket(HashBucket *hashBucket);

void freeHashTable(HashTable *hashTable);

int closestPrime(int num);

void printHashingBenchmarks();

HashTable *getMainHTable();

Vector *getMatchesVector();

Vector *getMissMatchesVector();

#endif