#ifndef _HASHWPLUS_H
#define _HASHWPLUS_H

#include "../hdr/clique.h"
#include "../hdr/readCSV.h"

typedef struct HashTable_w HashTable_w;
typedef struct HashBucket_w HashBucket_w;
typedef struct PairInfo_w PairInfo_w;
typedef struct Observation Observation;

struct HashTable_w
{
    long size;
    long itemsInserted;
    HashBucket_w **hashArray;
};

struct HashBucket_w
{
    Observation *observationsList;
};

struct Observation
{
    char *leftSpecId;
    char *rightSpecId;
    Vector *left_tf_idf;
    Vector *right_tf_idf;
    double isMatch;
    Observation *next;
};

HashTable_w *initHashTable_w(int pairsSum);

unsigned long long hashFunction_w(char *string1, char *string2);

void addToHashTable_w(HashTable_w *hashTable, Observation *newObservation);

void insertInChain_w(HashBucket_w *bucketDst, Observation *newObservation);

Observation *initObservation(char *leftSpecId, char *rightSpecId, int isMatch);

Observation *searchHashTable_w(HashTable_w *hashTable, char *leftSpecId, char *rightSpecId);

Observation *searchChain_w(Observation *observationListHead, char *leftSpecId, char *rightSpecId);

void freeObservationsList(Observation *listPtr);

void freeHashBucket_w(HashBucket_w *hashBucket);

void freeHashTable_w(HashTable_w *hashTable);

void createPairDatasets();

HashTable_w *getTrainingSet();

HashTable_w *getEvaluationSet();

HashTable_w *getTestSet();

#endif