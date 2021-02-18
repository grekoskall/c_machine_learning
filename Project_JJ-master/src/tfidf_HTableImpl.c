#include "../hdr/includes.h"

HashTable_gen *idfHTable;

double totalSpecs;
HashTable_gen *stopwordsHTable;

////////// INITIALIZATIONS //////////////////
tf_idfInfo *initTFIDFinfo(char *word)
{
    tf_idfInfo *newInfo = (tf_idfInfo *)safe_malloc(sizeof(tf_idfInfo));
    newInfo->word = createString(word);
    newInfo->tf_idfValue = 0.0;
    newInfo->next = NULL;

    return newInfo;
}

idfInfo *initIDFinfo(char *word)
{
    idfInfo *newInfo = (idfInfo *)safe_malloc(sizeof(idfInfo));
    newInfo->word = createString(word);
    newInfo->idfValue = 0.0;
    newInfo->next = NULL;

    return newInfo;
}
//////////////

////////// SEARCH FUNCTIONS FOR TF_IDF AND IDF HTABLES //////////////////
tf_idfInfo *searchTFIDF_htable(HashTable_gen *hashTable, char *word)
{
    int posInHTable = hashFunction(word) % hashTable->size;
    if (hashTable->hashArray[posInHTable] == NULL)
        return NULL;
    return searchTFIDF_htableChain(hashTable->hashArray[posInHTable]->bucketList, word);
}

tf_idfInfo *searchTFIDF_htableChain(tf_idfInfo *listHead, char *word)
{
    tf_idfInfo *listPtr = listHead;
    while (listPtr != NULL)
    {
        char *currentWord = listPtr->word;
        if (same_string(currentWord, word))
        { // found
            break;
        }
        listPtr = listPtr->next;
    }
    return listPtr;
}

idfInfo *searchIDF_htable(HashTable_gen *hashTable, char *word)
{
    int posInHTable = hashFunction(word) % hashTable->size;
    if (hashTable->hashArray[posInHTable] == NULL)
        return NULL;
    return searchIDF_htableChain(hashTable->hashArray[posInHTable]->bucketList, word);
}

idfInfo *searchIDF_htableChain(idfInfo *listHead, char *word)
{
    idfInfo *listPtr = listHead;
    while (listPtr != NULL)
    {
        char *currentWord = listPtr->word;
        if (same_string(currentWord, word))
        { // found
            break;
        }
        listPtr = listPtr->next;
    }
    return listPtr;
}
///////////////////////

////////// INSERT FUNCTIONS FOR TF_IDF AND IDF HTABLES //////////////////
void addToTFIDF_htable(HashTable_gen *hashTable, char *word)
{
    unsigned long long hash = hashFunction(word);
    long posInHTable = hash % hashTable->size;
    if (hashTable->hashArray[posInHTable] == NULL)
    {
        hashTable->hashArray[posInHTable] = (HashBucket_gen *)safe_malloc(sizeof(HashBucket_gen));
        hashTable->hashArray[posInHTable]->bucketList = NULL;
    }
    int insertedNewFlag = insertInTFIDFchain(hashTable->hashArray[posInHTable], word);
    if (insertedNewFlag == 1)
        hashTable->itemsInserted++;
}

int insertInTFIDFchain(HashBucket_gen *bucketDst, char *word)
{
    tf_idfInfo *listPtr = bucketDst->bucketList;
    if (listPtr == NULL)
    {
        tf_idfInfo *newInfo = initTFIDFinfo(word);
        newInfo->tf_idfValue++;
        bucketDst->bucketList = newInfo;
        addToIDF_htable(idfHTable, word);
        return 1;
    }
    tf_idfInfo *prevPtr = NULL;
    while (listPtr != NULL)
    {
        char *currentWord = listPtr->word;
        if (same_string(currentWord, word))
        { // found in list
            listPtr->tf_idfValue++;
            return 0;
        }

        prevPtr = listPtr;
        listPtr = listPtr->next;
    }
    // not found in list
    tf_idfInfo *newInfo = initTFIDFinfo(word);
    newInfo->tf_idfValue++;
    prevPtr->next = newInfo;
    addToIDF_htable(idfHTable, word);
    return 1;
}
////////////////////////

void addToIDF_htable(HashTable_gen *hashTable, char *word)
{
    unsigned long long hash = hashFunction(word);
    long posInHTable = hash % hashTable->size;
    if (hashTable->hashArray[posInHTable] == NULL)
    {
        hashTable->hashArray[posInHTable] = (HashBucket_gen *)safe_malloc(sizeof(HashBucket_gen));
        hashTable->hashArray[posInHTable]->bucketList = NULL;
    }
    int insertedNewFlag = insertInIDFchain(hashTable->hashArray[posInHTable], word);
    if (insertedNewFlag == 1)
        hashTable->itemsInserted++;
}

int insertInIDFchain(HashBucket_gen *bucketDst, char *word)
{
    idfInfo *listPtr = bucketDst->bucketList;
    if (listPtr == NULL)
    {
        idfInfo *newInfo = initIDFinfo(word);
        newInfo->idfValue++;
        bucketDst->bucketList = newInfo;
        return 1;
    }
    idfInfo *prevPtr = NULL;
    while (listPtr != NULL)
    {
        char *currentWord = listPtr->word;
        if (same_string(currentWord, word))
        { // found in list
            listPtr->idfValue++;
            return 0;
        }

        prevPtr = listPtr;
        listPtr = listPtr->next;
    }
    // not found in list
    idfInfo *newInfo = initIDFinfo(word);
    newInfo->idfValue++;
    prevPtr->next = newInfo;
    return 1;
}
////////////////////////

///////////////// CALCULATIONS FOR FINAL TF_IDF VALUES ///////////////////////
void calculateFinalIDF(HashTable_gen *hashTable)
{
    int htableSize = hashTable->size;
    idfInfo *listPtr = NULL;
    for (int i = 0; i < htableSize; i++)
    {
        if (hashTable->hashArray[i] == NULL)
            continue;
        listPtr = hashTable->hashArray[i]->bucketList;
        while (listPtr != NULL)
        {
            listPtr->idfValue = log(totalSpecs / listPtr->idfValue);

            listPtr = listPtr->next;
        }
    }
}

Vector *calculateFinalTFIDFforSpec(HashTable_gen *hashTable)
{
    long totalWords = hashTable->itemsInserted;
    int htableSize = hashTable->size;
    tf_idfInfo *listPtr = NULL;
    Vector *tfidfVector = vectorInit();
    for (int i = 0; i < htableSize; i++)
    {
        if (hashTable->hashArray[i] == NULL)
            continue;
        listPtr = hashTable->hashArray[i]->bucketList;
        while (listPtr != NULL)
        {
            listPtr->tf_idfValue = listPtr->tf_idfValue / totalWords;
            idfInfo *idfNodeForWord = searchIDF_htable(idfHTable, listPtr->word);
            vectorPushBack(tfidfVector, listPtr);
            if (idfNodeForWord == NULL)
                continue;
            listPtr->tf_idfValue = listPtr->tf_idfValue * idfNodeForWord->idfValue;

            listPtr = listPtr->next;
        }
    }
    // calculations done. Let's get the best 1.000 words from tf_idf
    sortTFIDFvector(tfidfVector);
    return tfidfVector;
}

void sortTFIDFvector(Vector *tfidfVector)
{ // let's do a selection sort
    int i, j, min_idx;
    int itemsCount = vectorItemsCount(tfidfVector);

    // One by one move boundary of unsorted subarray
    for (i = 0; i < itemsCount - 1; i++)
    {
        if (i > TF_IDF_SIZE)
            break;
        // Find the maximum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < itemsCount; j++)
            if (((tf_idfInfo *)tfidfVector->items[j])->tf_idfValue > ((tf_idfInfo *)tfidfVector->items[min_idx])->tf_idfValue)
                min_idx = j;

        // Swap the found maximum element with the first element
        tf_idfInfo *tfidfInfoToSwap = (tf_idfInfo *)tfidfVector->items[i];
        tfidfVector->items[i] = (tf_idfInfo *)tfidfVector->items[min_idx];
        tfidfVector->items[min_idx] = tfidfInfoToSwap;
    }
}
///////////////////

//////////////// FREE FUNCTIONS FOR BOTH HTABLES //////////////////////
void freeIDFInfoList(idfInfo *listPtr)
{
    if (listPtr == NULL)
        return;

    freeIDFInfoList(listPtr->next);
    free(listPtr->word);
    free(listPtr);
}

void freeIDFHashBucket(HashBucket_gen *hashBucket)
{
    if (hashBucket == NULL)
    {
        return;
    }
    freeIDFInfoList(hashBucket->bucketList);
    free(hashBucket);
}

void freeIDFHashTable(HashTable_gen *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        freeIDFHashBucket(hashTable->hashArray[i]);
    }
    free(hashTable->hashArray);
    free(hashTable);
}
/////////////////////////

void freeTFIDFInfoList(tf_idfInfo *listPtr)
{
    if (listPtr == NULL)
        return;

    freeTFIDFInfoList(listPtr->next);
    free(listPtr->word);
    free(listPtr);
}

void freeTFIDFHashBucket(HashBucket_gen *hashBucket)
{
    if (hashBucket == NULL)
    {
        return;
    }
    freeTFIDFInfoList(hashBucket->bucketList);
    free(hashBucket);
}

void freeTFIDFHashTable(HashTable_gen *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        freeTFIDFHashBucket(hashTable->hashArray[i]);
    }
    free(hashTable->hashArray);
    free(hashTable);
}
///////////////////

/////////////////////// MAIN FUNCTIONS /////////////////////////
void createTFIDFvectors(HashTable *mainHTable, HashTable_gen *stopwordsHTable_)
{
    if (mainHTable == NULL)
        return;
    puts("==> Creating the tf-idf Vectors for each Spec ...");

    idfHTable = initHashTable_gen(AVG_TOTAL_WORDS);
    stopwordsHTable = stopwordsHTable_;
    totalSpecs = 0;
    long htableSize = mainHTable->size;
    for (int i = 0; i < htableSize; i++)
    {
        if (mainHTable->hashArray[i] == NULL)
            continue;
        createTFIDFvectorsInChain(mainHTable->hashArray[i]->specList);
    }
    calculateFinalIDF(idfHTable);

    for (int i = 0; i < htableSize; i++)
    {
        if (mainHTable->hashArray[i] == NULL)
            continue;
        calculateFinalTFIDFinChain(mainHTable->hashArray[i]->specList);
    }

    freeIDFHashTable(idfHTable);
}

void calculateFinalTFIDFinChain(SpecNode *head)
{
    SpecNode *specPtr = head;
    SpecInfo *currentSpecInfo = NULL;
    while (specPtr != NULL)
    {
        //addSpecToTFIDF_HTables(specPtr);
        currentSpecInfo = specPtr->cliquePtr->specInfo;
        currentSpecInfo->tfidf_vector = calculateFinalTFIDFforSpec(currentSpecInfo->tfidf_HTable);
        specPtr = specPtr->nextSpec;
    }
}

void createTFIDFvectorsInChain(SpecNode *head)
{
    SpecNode *specPtr = head;
    while (specPtr != NULL)
    {
        addSpecToTFIDF_HTables(specPtr);
        specPtr = specPtr->nextSpec;
    }
}

void addSpecToTFIDF_HTables(SpecNode *specNode)
{
    totalSpecs++;
    SpecInfo *specInfo = specNode->cliquePtr->specInfo;
    if (specInfo == NULL)
        return;
    specInfo->tfidf_HTable = initHashTable_gen(AVG_WORDS_IN_JSON);
    processWordAddToTFIDF_HTables(specInfo->tfidf_HTable, "page");
    processWordAddToTFIDF_HTables(specInfo->tfidf_HTable, "title");
    addSentenceToTFIDF_HTables(specInfo->tfidf_HTable, specInfo->pageTitle);
    traverseInfoListToTFIDF_HTables(specInfo->tfidf_HTable, specInfo->infoList);
}

void processWordAddToTFIDF_HTables(HashTable_gen *tfidfHTable, char *word)
{
    char *wordCopy = createString(word);
    for (int i = 0; i < strlen(wordCopy); i++)
    {
        wordCopy[i] = tolower(wordCopy[i]);
    }
    if (isInStopwords(stopwordsHTable, wordCopy))
    {
        free(wordCopy);
        return;
    }
    addToTFIDF_htable(tfidfHTable, wordCopy);
    free(wordCopy);
}

void addSentenceToTFIDF_HTables(HashTable_gen *tfidfHTable, char *sentence)
{
    char delim[15] = {
        ' ',
        ',',
        '.',
        ':',
        '\n',
        '&',
        '\\',
        '|',
        '/',
        '-',
        '(',
        ')',
        '!',
        '?',
        '=',
    };
    char *ptr = strtok(sentence, delim);
    while (ptr != NULL)
    {
        processWordAddToTFIDF_HTables(tfidfHTable, ptr);
        ptr = strtok(NULL, delim);
    }
}

void traverseInfoListToTFIDF_HTables(HashTable_gen *tfidfHTable, InfoList *infoList)
{
    InfoNode *tmpInfoNode = infoList->head;
    while (tmpInfoNode != NULL)
    {
        addSentenceToTFIDF_HTables(tfidfHTable, tmpInfoNode->description);
        addSentenceToTFIDF_HTables(tfidfHTable, tmpInfoNode->content);

        tmpInfoNode = tmpInfoNode->next;
    }
}

void printTFIDFvectors(HashTable *mainHTable)
{
    for (int i = 0; i < mainHTable->size; i++)
    {
        if (mainHTable->hashArray[i] == NULL)
            continue;
        printTFIDFvectorInChain(mainHTable->hashArray[i]->specList);
    }
}

void printTFIDFvectorInChain(SpecNode *head)
{
    SpecNode *listPtr = head;
    while (listPtr != NULL)
    {
        if (vectorItemsCount(listPtr->cliquePtr->specInfo->tfidf_vector) >= 1000)
        {
            printf("specId: %s\n", listPtr->cliquePtr->specInfo->specId);
        }
        printfTFIDFvectorForSpec(listPtr->cliquePtr->specInfo->tfidf_vector);
        listPtr = listPtr->nextSpec;
    }
}

void printfTFIDFvectorForSpec(Vector *tfidfVector)
{
    int itemsCount = vectorItemsCount(tfidfVector);
    for (int i = 0; i < itemsCount; i++)
    {
        tf_idfInfo *currentWordInfo = (tf_idfInfo *)vectorGet(tfidfVector, i);
        printf("%s : %f || ", currentWordInfo->word, currentWordInfo->tf_idfValue);
    }
    puts(" ");
    puts("== NEXT ==");
}