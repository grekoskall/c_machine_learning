#include "../hdr/includes.h"

HashTable_gen *saveStopwords(char *fileName)
{
    FILE *stream = fopen(fileName, "r");
    if (stream == NULL)
    {
        printf("Could not create %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    HashTable_gen *stopWordsHTable = initHashTable_gen(40);
    char line[MAXLINE];
    char *nextField = NULL;
    while (fgets(line, MAXLINE, stream))
    {
        for (nextField = strtok(line, ","); nextField != NULL; nextField = strtok(NULL, ","))
        {
            addToStopwordsHTable(stopWordsHTable, nextField);
        }
    }

    fclose(stream);
    return stopWordsHTable;
}

stopwordNode *initStopword(char *word)
{
    stopwordNode *newStopword = (stopwordNode *)safe_malloc(sizeof(stopwordNode));
    newStopword->word = createString(word);
    newStopword->next = NULL;

    return newStopword;
}

int isInStopwords(HashTable_gen *hashTable, char *word)
{
    int posInHTable = hashFunction(word) % hashTable->size;
    if (hashTable->hashArray[posInHTable] == NULL)
        return 0;
    return isInStopwordsChain(hashTable->hashArray[posInHTable]->bucketList, word);
}

int isInStopwordsChain(stopwordNode *listHead, char *word)
{
    stopwordNode *listPtr = listHead;
    while (listPtr != NULL)
    {
        char *currentWord = listPtr->word;
        if (same_string(currentWord, word))
        { // found
            return 1;
        }
        listPtr = listPtr->next;
    }
    return 0;
}

void addToStopwordsHTable(HashTable_gen *hashTable, char *word)
{
    unsigned long long hash = hashFunction(word);
    long posInHTable = hash % hashTable->size;
    if (hashTable->hashArray[posInHTable] == NULL)
    {
        hashTable->hashArray[posInHTable] = (HashBucket_gen *)safe_malloc(sizeof(HashBucket_gen));
        hashTable->hashArray[posInHTable]->bucketList = NULL;
    }
    insertInStopwordsChain(hashTable->hashArray[posInHTable], word);
}

void insertInStopwordsChain(HashBucket_gen *bucketDst, char *word)
{
    stopwordNode *listPtr = bucketDst->bucketList;
    if (listPtr == NULL)
    {
        stopwordNode *newStopword = initStopword(word);
        bucketDst->bucketList = newStopword;
        return;
    }
    while (listPtr->next != NULL)
    {
        listPtr = listPtr->next;
    }
    stopwordNode *newStopword = initStopword(word);
    listPtr->next = newStopword;
}

void freeStopWordsList(stopwordNode *listPtr)
{
    if (listPtr == NULL)
        return;

    freeStopWordsList(listPtr->next);
    free(listPtr->word);
    free(listPtr);
}

void freeStopwordsHashBucket(HashBucket_gen *hashBucket)
{
    if (hashBucket == NULL)
    {
        return;
    }
    freeStopWordsList(hashBucket->bucketList);
    free(hashBucket);
}

void freeStopwordsHTable(HashTable_gen *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        freeStopwordsHashBucket(hashTable->hashArray[i]);
    }
    free(hashTable->hashArray);
    free(hashTable);
}