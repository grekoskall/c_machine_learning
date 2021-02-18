#include "../hdr/includes.h"

HashTable_w *trainingSet_HTable;
HashTable_w *evaluationSet_HTable;
HashTable_w *testSet_HTable;

HashTable_w *
initHashTable_w(int pairsSum)
{
    int bucketsToAlloc = pairsSum / 0.5; // we need a load factor of at least 70%, to minimize collisions
                                         // let's double the size here
    bucketsToAlloc = closestPrime(bucketsToAlloc);

    HashTable_w *newHashTable = (HashTable_w *)safe_malloc(sizeof(HashTable_w));
    newHashTable->size = bucketsToAlloc;
    newHashTable->itemsInserted = 0;
    newHashTable->hashArray = (HashBucket_w **)safe_calloc(bucketsToAlloc, sizeof(HashBucket_w *));

    for (int i = 0; i < bucketsToAlloc; i++)
    {
        newHashTable->hashArray[i] = NULL;
    }
    return newHashTable;
}

unsigned long long hashFunction_w(char *string1, char *string2)
{
    unsigned long long h = 0;
    long len = strlen(string1);
    for (int i = 0; i < len; i++)
    {
        h = h + 7 * string1[i];
    }
    len = strlen(string2);
    for (int i = 0; i < len; i++)
    {
        h = h + 7 * string2[i];
    }

    return h;
}

void addToHashTable_w(HashTable_w *hashTable, Observation *newObservation)
{
    hashTable->itemsInserted++;
    unsigned long long hash = hashFunction_w(newObservation->leftSpecId, newObservation->rightSpecId);
    long posInHashTable = hash % hashTable->size;

    if (hashTable->hashArray[posInHashTable] == NULL)
    {
        hashTable->hashArray[posInHashTable] = (HashBucket_w *)safe_malloc(sizeof(HashBucket_w));
        hashTable->hashArray[posInHashTable]->observationsList = NULL;
    }
    insertInChain_w(hashTable->hashArray[posInHashTable], newObservation);
}

void insertInChain_w(HashBucket_w *bucketDst, Observation *newObservation)
{
    if (bucketDst->observationsList == NULL)
    {
        bucketDst->observationsList = newObservation;
        return;
    }

    Observation *observationPtrList = bucketDst->observationsList;
    while (observationPtrList->next != NULL)
    {
        observationPtrList = observationPtrList->next;
    }
    observationPtrList->next = newObservation;
}

Observation *initObservation(char *leftSpecId, char *rightSpecId, int isMatch)
{
    Observation *newObservation = (Observation *)safe_malloc(sizeof(Observation));
    newObservation->leftSpecId = createString(leftSpecId);
    newObservation->rightSpecId = createString(rightSpecId);
    newObservation->isMatch = isMatch;
    SpecInfo *leftspecInfo = searchHashTable(getMainHTable(), leftSpecId)->cliquePtr->specInfo;
    newObservation->left_tf_idf = leftspecInfo->tfidf_vector;
    SpecInfo *rightSpecInfo = searchHashTable(getMainHTable(), rightSpecId)->cliquePtr->specInfo;
    newObservation->right_tf_idf = rightSpecInfo->tfidf_vector;
    newObservation->next = NULL;

    return newObservation;
}

Observation *searchHashTable_w(HashTable_w *hashTable, char *leftSpecId, char *rightSpecId)
{
    int posInHashTable = hashFunction_w(leftSpecId, rightSpecId) % hashTable->size;
    if (hashTable->hashArray[posInHashTable] == NULL || hashTable->hashArray[posInHashTable]->observationsList == NULL )
        return NULL;
    return searchChain_w(hashTable->hashArray[posInHashTable]->observationsList, leftSpecId, rightSpecId);
}

Observation *searchChain_w(Observation *observationListHead, char *leftSpecId, char *rightSpecId)
{
    Observation *listPtr = observationListHead;
    while (listPtr != NULL)
    {
        /*char *string1 = listPtr->leftSpecId;
        char *string2 = listPtr->rightSpecId;
        if ((same_string(leftSpecId, string1) && same_string(rightSpecId, string2)) || (same_string(leftSpecId, string2) && same_string(rightSpecId, string1)))
        { // found
            break;
        }*/
        if ( listPtr->leftSpecId == NULL || listPtr->rightSpecId == NULL ) return NULL;
        if ( strcmp(leftSpecId, listPtr->leftSpecId) == 0 && strcmp(rightSpecId, listPtr->rightSpecId) == 0 ) break;
        listPtr = listPtr->next;
    }
    return listPtr;
}

void freeObservationsList(Observation *listPtr)
{
    if (listPtr == NULL)
        return;
    freeObservationsList(listPtr->next);
    free(listPtr->leftSpecId);
    free(listPtr->rightSpecId);
    free(listPtr);
}

void freeHashBucket_w(HashBucket_w *hashBucket)
{
    if (hashBucket == NULL)
    {
        return;
    }
    freeObservationsList(hashBucket->observationsList);
    free(hashBucket);
}

void freeHashTable_w(HashTable_w *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        freeHashBucket_w(hashTable->hashArray[i]);
    }
    free(hashTable->hashArray);
    free(hashTable);
}

/**
 * Mix matches and miss-matches pairs
 * */
void createPairDatasets()
{
    Vector *matchesVector = getMatchesVector();
    Vector *missMatchesVEctor = getMissMatchesVector();
    int totalMatches = vectorItemsCount(matchesVector);
    int totalMissMatches = vectorItemsCount(missMatchesVEctor);
    int trainingSetPairs = (totalMatches + totalMissMatches) * 0.6;
    int evaluationSetPairs = (totalMatches + totalMissMatches) * 0.2;
    int testSetPairs = (totalMatches + totalMissMatches) - trainingSetPairs - evaluationSetPairs;
    trainingSet_HTable = initHashTable_w(trainingSetPairs);
    evaluationSet_HTable = initHashTable_w(evaluationSetPairs);
    testSet_HTable = initHashTable_w(testSetPairs);
    int matchesCounter = 0, missMatchesCounter = 0, totalPairsInserted = 0;
    HashTable_w *htableToInsert = trainingSet_HTable;
    for (int i = 0; i < totalMissMatches; i++)
    {
        missMatchesCounter++;
        Observation *currObservation = vectorGet(missMatchesVEctor, i);
        if (totalPairsInserted > trainingSetPairs)
        {
            if (totalPairsInserted > (trainingSetPairs + evaluationSetPairs))
                htableToInsert = testSet_HTable;
            else
                htableToInsert = evaluationSet_HTable;
        }
        if (1)
        {
            addToHashTable_w(htableToInsert, currObservation);
            totalPairsInserted++;
        }
        if ((missMatchesCounter >= 5) && (matchesCounter < totalMatches))
        {
            currObservation = vectorGet(matchesVector, matchesCounter);
            if (totalPairsInserted > trainingSetPairs)
            {
                if (totalPairsInserted > (trainingSetPairs + evaluationSetPairs))
                    htableToInsert = testSet_HTable;
                else
                    htableToInsert = evaluationSet_HTable;
            }
            if (1)
            {
                addToHashTable_w(htableToInsert, currObservation);
                totalPairsInserted++;
            }
            matchesCounter++;
            missMatchesCounter = 0;
        }
    }

    freeVectorWithoutItems(getMatchesVector());
    freeVectorWithoutItems(getMissMatchesVector());
}

HashTable_w *getTrainingSet()
{
    return trainingSet_HTable;
}

HashTable_w *getEvaluationSet()
{
    return evaluationSet_HTable;
}

HashTable_w *getTestSet()
{
    return testSet_HTable;
}