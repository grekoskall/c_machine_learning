#ifndef _STOPWORDS_H
#define _STOPWORDS_H

typedef struct HashTable_gen HashTable_gen;
typedef struct HashBucket_gen HashBucket_gen;
typedef struct stopwordNode stopwordNode;

struct stopwordNode
{
    char *word;
    stopwordNode *next;
};

HashTable_gen *saveStopwords(char *fileName);

stopwordNode *initStopword(char *word);

int isInStopwords(HashTable_gen *hashTable, char *word);

int isInStopwordsChain(stopwordNode *listHead, char *word);

void addToStopwordsHTable(HashTable_gen *hashTable, char *word);

void insertInStopwordsChain(HashBucket_gen *bucketDst, char *word);

void freeStopWordsList(stopwordNode *listPtr);

void freeStopwordsHashBucket(HashBucket_gen *hashBucket);

void freeStopwordsHTable(HashTable_gen *hashTable);

#endif