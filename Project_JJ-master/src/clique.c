#include "../hdr/includes.h"

long matchesFound = 0;
long missMatchesFound = 0;

CliqueNode *initCliqueNode()
{
    CliqueNode *newCliqueNode = (CliqueNode *)safe_malloc(sizeof(CliqueNode));
    newCliqueNode->specInfo = NULL;
    newCliqueNode->next = newCliqueNode; // we have a cyclic list
    newCliqueNode->prev = newCliqueNode;
    newCliqueNode->cliqueMissMatchVector = NULL;
    newCliqueNode->isPrinted = 0;
    newCliqueNode->hasPrintedMissMatches = 0;

    return newCliqueNode;
}

void updateCliques(char *leftSpecId, char *rightSpecId, HashTable *hashTable)
{
    CliqueNode *leftClique = getCliqueNode(leftSpecId, hashTable);
    CliqueNode *rightClique = getCliqueNode(rightSpecId, hashTable);
    if (leftClique == NULL || rightClique == NULL)
        return;
    makeCliqueAdjustmnets(leftClique, rightClique);
}

CliqueNode *getCliqueNode(char *specId, HashTable *hashTable)
{
    SpecNode *mySpecNode = searchHashTable(hashTable, specId);
    if (mySpecNode == NULL)
        return NULL;
    return mySpecNode->cliquePtr;
}

void makeCliqueAdjustmnets(CliqueNode *leftClique, CliqueNode *rightClique)
{
    if (leftClique == NULL || rightClique == NULL)
        return;
    if (alreadyInSameClique(leftClique, rightClique) == 1)
    {
        return;
    }
    rightClique->prev->next = leftClique->next;
    leftClique->next->prev = rightClique->prev;
    leftClique->next = rightClique;
    rightClique->prev = leftClique;
    mergeCliqueMissMatchVectors(leftClique, rightClique);
}

void mergeCliqueMissMatchVectors(CliqueNode *leftClique, CliqueNode *rightClique)
{
    if (leftClique->cliqueMissMatchVector == NULL && rightClique->cliqueMissMatchVector == NULL)
    {
        Vector *newCliqueMissMatchVector = vectorInit();
        leftClique->cliqueMissMatchVector = newCliqueMissMatchVector;
        rightClique->cliqueMissMatchVector = newCliqueMissMatchVector;
        return;
    }
    if (leftClique->cliqueMissMatchVector == NULL)
    {
        leftClique->cliqueMissMatchVector = rightClique->cliqueMissMatchVector;
        return;
    }
    if (rightClique->cliqueMissMatchVector == NULL)
    {
        rightClique->cliqueMissMatchVector = leftClique->cliqueMissMatchVector;
        return;
    }
    Vector *newMergedMissMatchVector = vectorInit();
    copyToEmptyMissMatchVector(newMergedMissMatchVector, leftClique->cliqueMissMatchVector);
    int newVectorItemsCount = vectorItemsCount(newMergedMissMatchVector);
    int leftVectorItemsCount = vectorItemsCount(leftClique->cliqueMissMatchVector);
    int alreadyInVector = 0;
    for (int i = 0; i < leftVectorItemsCount; i++)
    {
        alreadyInVector = 0;
        char *specIdToBeMerged = ((CliqueNode *)leftClique->cliqueMissMatchVector->items[i])->specInfo->specId;
        for (int j = 0; j < newVectorItemsCount; j++)
        {
            char *specIdInNewVector = ((CliqueNode *)newMergedMissMatchVector->items[j])->specInfo->specId;
            if (same_string(specIdToBeMerged, specIdInNewVector))
            {
                alreadyInVector = 1;
                break;
            }
        }
        if (alreadyInVector == 0)
        {
            vectorPushBack(newMergedMissMatchVector, leftClique->cliqueMissMatchVector->items[i]);
        }
    }

    freeVectorWithoutItems(leftClique->cliqueMissMatchVector);
    freeVectorWithoutItems(rightClique->cliqueMissMatchVector);
    leftClique->cliqueMissMatchVector = newMergedMissMatchVector;
    CliqueNode *tmpCliqueNode = leftClique->next;
    while (strcmp(tmpCliqueNode->specInfo->specId, leftClique->specInfo->specId) != 0)
    {
        tmpCliqueNode->cliqueMissMatchVector = newMergedMissMatchVector;
        tmpCliqueNode = tmpCliqueNode->next;
    }
}

void updateMissMatchCliques(char *leftSpecId, char *rightSpecId, HashTable *hashTable)
{
    CliqueNode *leftClique = getCliqueNode(leftSpecId, hashTable);
    CliqueNode *rightClique = getCliqueNode(rightSpecId, hashTable);
    if (leftClique == NULL || rightClique == NULL)
        return;
    updateMissMatchVector(leftClique, rightClique);
    updateMissMatchVector(rightClique, leftClique);
}

void updateMissMatchVector(CliqueNode *srcClique, CliqueNode *missMatchClique)
{
    Vector *cliqueMissMatchVector = srcClique->cliqueMissMatchVector;
    if (cliqueMissMatchVector == NULL)
    {
        Vector *newMissMatchVector = vectorInit();
        vectorPushBack(newMissMatchVector, missMatchClique);
        srcClique->cliqueMissMatchVector = newMissMatchVector;
        return;
    }
    int itemsCount = vectorItemsCount(srcClique->cliqueMissMatchVector);
    for (int i = 0; i < itemsCount; i++)
    {
        char *currentSpecId = ((CliqueNode *)srcClique->cliqueMissMatchVector->items[i])->specInfo->specId;
        if (same_string(currentSpecId, missMatchClique->specInfo->specId))
        { // Vector already contains this miss match
            return;
        }
    }
    vectorPushBack(srcClique->cliqueMissMatchVector, missMatchClique);
}

void removeFromClique(CliqueNode *cliqueNode)
{
    cliqueNode->prev->next = cliqueNode->next;
    cliqueNode->next->prev = cliqueNode->prev;
    cliqueNode->next = cliqueNode;
    cliqueNode->prev = cliqueNode;
}

void printSpecMatches(SpecNode *specNode, FILE *fptr)
{
    CliqueNode *cliqueNode = specNode->cliquePtr;
    CliqueNode *cliquePtr = cliqueNode->next;
    while (strcmp(cliquePtr->specInfo->specId, cliqueNode->specInfo->specId) != 0)
    {
        if (cliquePtr->isPrinted == 1)
        {
            cliquePtr = cliquePtr->next;
            continue;
        }
        matchesFound++;
        //printf("%s , %s\n", cliqueNode->specInfo->specId, cliquePtr->specInfo->specId);
        Observation *newObservation = initObservation(cliqueNode->specInfo->specId, cliquePtr->specInfo->specId, 1);
        vectorPushBack(getMatchesVector(), newObservation);
        if (fptr != NULL)
            fprintf(fptr, "%s,%s\n", cliqueNode->specInfo->specId, cliquePtr->specInfo->specId);
        cliquePtr = cliquePtr->next;
    }
}

void printSpecMissMatches(SpecNode *specNode, FILE *fptr)
{
    CliqueNode *cliquePtr = specNode->cliquePtr;
    Vector *cliqueMissMatchVector = cliquePtr->cliqueMissMatchVector;
    int itemsCount = vectorItemsCount(cliqueMissMatchVector);
    for (int i = 0; i < itemsCount; i++)
    {
        CliqueNode *missMatchCliqueNode = (CliqueNode *)cliqueMissMatchVector->items[i];
        if (missMatchCliqueNode->hasPrintedMissMatches == 1)
        {
            continue;
        }
        missMatchesFound++;
        //printf("%s,%s\n", cliquePtr->specInfo->specId, missMatchCliqueNode->specInfo->specId);
        Observation *newObservation = initObservation(cliquePtr->specInfo->specId, missMatchCliqueNode->specInfo->specId, 0);
        vectorPushBack(getMissMatchesVector(), newObservation);
        if (fptr != NULL)
        {
            fprintf(fptr, "%s,%s\n", cliquePtr->specInfo->specId, missMatchCliqueNode->specInfo->specId);
        }
    }
}

int alreadyInSameClique(CliqueNode *leftCliqueNode, CliqueNode *rightCliqueNode)
{
    if (leftCliqueNode == NULL || rightCliqueNode == NULL)
        return 0;
    CliqueNode *cliquePtr = leftCliqueNode->next;
    while (strcmp(cliquePtr->specInfo->specId, leftCliqueNode->specInfo->specId) != 0)
    {
        if (strcmp(cliquePtr->specInfo->specId, rightCliqueNode->specInfo->specId) == 0)
        {
            return 1;
        }
        cliquePtr = cliquePtr->next;
    }

    return 0;
}

void copyToEmptyMissMatchVector(Vector *dstVector, Vector *srcVector)
{
    if (dstVector == NULL || srcVector == NULL || vectorItemsCount(dstVector) != 0)
    {
        return;
    }
    int srcItemsCount = vectorItemsCount(srcVector);
    for (int i = 0; i < srcItemsCount; i++)
    {
        vectorPushBack(dstVector, srcVector->items[i]);
    }
}

void resetAllPrintedStatus(HashTable *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        if (hashTable->hashArray[i] == NULL) // bucket not allocated, has no Specs
            continue;
        resetAllPrintedStatusInChain(hashTable->hashArray[i]->specList);
    }
}

void resetAllPrintedStatusInChain(SpecNode *head)
{
    SpecNode *specPtr = head;
    while (specPtr != NULL)
    {
        specPtr->cliquePtr->isPrinted = 0;
        specPtr->cliquePtr->hasPrintedMissMatches = 0;
        specPtr = specPtr->nextSpec;
    }
}

void printMatchesCount()
{
    printf("Matches found: %ld\n", matchesFound);
}

void printMissMatchesCount()
{
    printf("Miss-Matches found: %ld\n", missMatchesFound);
}

void freeCliqueNode(CliqueNode *cliqueNode)
{
    if (cliqueNode == NULL)
        return;
    freeSpecInfo(cliqueNode->specInfo);
    free(cliqueNode);
}

void freeMissMatchVector(HashBucket *hashBucket)
{
    if (hashBucket == NULL)
        return;

    SpecNode *currentSpecNode = hashBucket->specList;
    CliqueNode *currentCliqueNode;
    while (currentSpecNode != NULL)
    {
        currentCliqueNode = currentSpecNode->cliquePtr;
        CliqueNode *tmpCliqueNode = currentCliqueNode->next;
        while (strcmp(tmpCliqueNode->specInfo->specId, currentCliqueNode->specInfo->specId) != 0)
        {
            tmpCliqueNode->cliqueMissMatchVector = NULL;
            tmpCliqueNode = tmpCliqueNode->next;
        }
        freeVectorWithoutItems(currentCliqueNode->cliqueMissMatchVector);

        currentSpecNode = currentSpecNode->nextSpec;
    }
}
