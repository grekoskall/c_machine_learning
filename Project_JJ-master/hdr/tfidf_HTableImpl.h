#ifndef _TFIDFHASH_H
#define _TFIDFHASH_H

#define TF_IDF_SIZE 1000
#define AVG_WORDS_IN_JSON 400
#define AVG_TOTAL_WORDS 60000

//typedef struct MLInfo MLInfo;
typedef struct idfInfo idfInfo;
//typedef struct tfInfo tfInfo;
typedef struct tf_idfInfo tf_idfInfo;

struct tf_idfInfo
{
    char *word;
    double tf_idfValue;
    tf_idfInfo *next;
};

struct idfInfo
{
    char *word;
    double idfValue;
    idfInfo *next;
};

tf_idfInfo *initTFIDFinfo(char *word);

idfInfo *initIDFinfo(char *word);

tf_idfInfo *searchTFIDF_htable(HashTable_gen *hashTable, char *word);

tf_idfInfo *searchTFIDF_htableChain(tf_idfInfo *listHead, char *word);

idfInfo *searchIDF_htable(HashTable_gen *hashTable, char *word);

idfInfo *searchIDF_htableChain(idfInfo *listHead, char *word);

void addToTFIDF_htable(HashTable_gen *hashTable, char *word);

int insertInTFIDFchain(HashBucket_gen *bucketDst, char *word);

void addToIDF_htable(HashTable_gen *hashTable, char *word);

int insertInIDFchain(HashBucket_gen *bucketDst, char *word);

void calculateFinalIDF(HashTable_gen *hashTable);

Vector *calculateFinalTFIDFforSpec(HashTable_gen *hashTable);

void sortTFIDFvector(Vector *tfidfVector);

void freeIDFInfoList(idfInfo *listPtr);

void freeIDFHashBucket(HashBucket_gen *hashBucket);

void freeIDFHashTable(HashTable_gen *hashTable);

void freeTFIDFInfoList(tf_idfInfo *listPtr);

void freeTFIDFHashBucket(HashBucket_gen *hashBucket);

void freeTFIDFHashTable(HashTable_gen *hashTable);

void createTFIDFvectors(HashTable *mainHTable, HashTable_gen *stopwordsHTable_);

void calculateFinalTFIDFinChain(SpecNode *head);

void createTFIDFvectorsInChain(SpecNode *head);

void addSpecToTFIDF_HTables(SpecNode *specNode);

void processWordAddToTFIDF_HTables(HashTable_gen *tfidfHTable, char *word);

void addSentenceToTFIDF_HTables(HashTable_gen *tfidfHTable, char *sentence);

void traverseInfoListToTFIDF_HTables(HashTable_gen *tfidfHTable, InfoList *infoList);

void printTFIDFvectors(HashTable *mainHTable);

void printTFIDFvectorInChain(SpecNode *head);

void printfTFIDFvectorForSpec(Vector *tfidfVector);

// struct idfInfo
// {
//     char *word;
//     double timesSeen;
//     int alreadySeenInSpecFlag;
//     double idfValue;
//     double tfSum;
//     double tf_idfValue;
// };

// struct tfInfo
// {
//     char *word;
//     double tfValue;
// };

#endif