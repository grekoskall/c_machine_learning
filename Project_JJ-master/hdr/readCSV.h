#ifndef _READCSV_H
#define _READCSV_H

#define MAXLINE 1024

#include "../hdr/hash.h"
#include "../hdr/vectorImpl.h"

typedef struct PairInfo PairInfo;

struct PairInfo
{
    char *leftSpecId;
    char *rightSpecId;
    int isMatch;
};

char *getField(char *line, int fieldNum);

void readDictionary(char *fileName, HashTable *hashTable);

Vector *readCsvToVector(char *fileName);

int countCSVlines(char *fileName);

PairInfo *initPairInfo(char *leftSpecId, char *rightSpecId, int isMatch);

Vector *getTrainingPairsVector();

Vector *getEvaluationPairsVector();

void freePairInfo(Vector *vectorPairInfo);

void freePairVector(Vector *pairVector);

#endif
