#include "../hdr/includes.h"

int countPrintedSpecs = 0;
HashTable *mainHTable = NULL;
Vector *matchesVector = NULL;
Vector *missMatchesVector = NULL;
/**
 * For Benchmarking use only!
 * */
int collisions = 0;
int insertions = 0;
int searchInChain = 0;
int bucketsCreated = 0;
int specNodesVisited = 0;
/////////////////

static int primes[72] = {
    3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
    1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591,
    17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
    187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263,
    1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369};

int closestPrime(int num)
{
    for (int i = 0; i < 72; i++)
    {
        if (num < primes[i])
        {
            return primes[i];
        }
    }

    return num; // to be safe
}

HashTable *initHashTable(int specSum)
{
    int bucketsToAlloc = specSum / 0.5; // we need a load factor of at least 70%, to minimize collisions
                                        // let's double the size here
    bucketsToAlloc = closestPrime(bucketsToAlloc);
    bucketsCreated = bucketsToAlloc; // for benchmarking only

    HashTable *hashTable = (HashTable *)safe_malloc(sizeof(HashTable));
    hashTable->size = bucketsToAlloc;
    hashTable->hashArray = (HashBucket **)safe_calloc(bucketsToAlloc, sizeof(HashBucket *));

    for (int i = 0; i < bucketsToAlloc; i++)
    {
        hashTable->hashArray[i] = NULL;
    }

    mainHTable = hashTable;
    return hashTable;
}

// Java's java.lang.String hashCode() method
unsigned long long hashFunction(char *specId)
{
    unsigned long long h = 0;
    long len = strlen(specId);
    for (int i = 0; i < len; i++)
    {
        h = 31 * h + specId[i];
    }

    return h;
}

void addToHashTable(HashTable *hashTable, SpecInfo *specToAdd)
{
    // call hash function, allocate specnode and cliqueNode and store them in hashArray
    unsigned long long hash = hashFunction(specToAdd->specId);
    long posInHashTable = hash % hashTable->size;
    SpecNode *newSpecNode = initSpecNode();
    CliqueNode *newCliqueNode = initCliqueNode();

    newCliqueNode->specInfo = specToAdd;
    newSpecNode->cliquePtr = newCliqueNode;
    if (hashTable->hashArray[posInHashTable] == NULL)
    {
        hashTable->hashArray[posInHashTable] = (HashBucket *)safe_malloc(sizeof(HashBucket));
        hashTable->hashArray[posInHashTable]->specList = NULL;
    }
    insertInChain(hashTable->hashArray[posInHashTable], newSpecNode);
}

void insertInChain(HashBucket *bucketDst, SpecNode *newSpecNode)
{
    insertions++;
    if (bucketDst->specList == NULL)
    {
        bucketDst->specList = newSpecNode;
        return;
    }
    collisions++;
    SpecNode *dstPtr = bucketDst->specList;
    while (dstPtr->nextSpec != NULL)
    {
        searchInChain++;
        dstPtr = dstPtr->nextSpec;
    }
    dstPtr->nextSpec = newSpecNode;
}

SpecNode *initSpecNode()
{
    SpecNode *newSpecNode = (SpecNode *)safe_malloc(sizeof(SpecNode));
    newSpecNode->cliquePtr = NULL;
    newSpecNode->nextSpec = NULL;

    return newSpecNode;
}

SpecNode *searchHashTable(HashTable *hashTable, char *specId)
{
    int posInHashTable = hashFunction(specId) % hashTable->size;
    if (hashTable->hashArray[posInHashTable] == NULL)
        return NULL;
    return searchChain(hashTable->hashArray[posInHashTable]->specList, specId);
}

SpecNode *searchChain(SpecNode *head, char *specId)
{
    SpecNode *specPtr = head;
    while (specPtr != NULL)
    {
        if (strcmp(specId, specPtr->cliquePtr->specInfo->specId) == 0) // found
        {
            break;
        }
        specPtr = specPtr->nextSpec;
    }
    return specPtr;
}

void printAllMatches(HashTable *hashTable, FILE *fptr)
{
    if (hashTable == NULL)
        return;
    matchesVector = vectorInit();
    for (int i = 0; i < hashTable->size; i++)
    {
        if (hashTable->hashArray[i] == NULL) // bucket not allocated, has no Specs
            continue;
        printSpecMatchesInChain(hashTable->hashArray[i]->specList, fptr);
    }
}

void printSpecMatchesInChain(SpecNode *head, FILE *fptr)
{
    SpecNode *specPtr = head;
    while (specPtr != NULL)
    {
        specNodesVisited++;
        printSpecMatches(specPtr, fptr);
        specPtr->cliquePtr->isPrinted = 1;
        //removeFromClique(specPtr->cliquePtr);
        specPtr = specPtr->nextSpec;
    }
}

void printAllMissMatches(HashTable *hashTable, FILE *fptr)
{
    if (hashTable == NULL)
        return;
    missMatchesVector = vectorInit();
    for (int i = 0; i < hashTable->size; i++)
    {
        if (hashTable->hashArray[i] == NULL) // bucket not allocated, has no Specs
            continue;
        printSpecMissMatchesInChain(hashTable->hashArray[i]->specList, fptr);
    }
}

void printSpecMissMatchesInChain(SpecNode *head, FILE *fptr)
{
    SpecNode *specPtr = head;
    while (specPtr != NULL)
    {
        printSpecMissMatches(specPtr, fptr);
        specPtr->cliquePtr->hasPrintedMissMatches = 1;
        specPtr = specPtr->nextSpec;
    }
}

void printHashTable(HashTable *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        if (hashTable->hashArray[i] == NULL) // bucket not allocated, has no Specs
            continue;
        printSpecChain(hashTable->hashArray[i]->specList);
    }
    printf("PRINTED SPECS: %d\n", countPrintedSpecs);
    countPrintedSpecs = 0;
}

void printSpecChain(SpecNode *head)
{
    SpecNode *specPtr = head;
    while (specPtr != NULL)
    {
        countPrintedSpecs++;
        print_spec(specPtr->cliquePtr->specInfo);
        specPtr = specPtr->nextSpec;
    }
}

void printVisitedSpecNodesCount()
{
    printf("Visited Spec nodes: %d\n", specNodesVisited);
}

void freeSpecNode(SpecNode *specNode)
{
    if (specNode == NULL)
        return;
    freeCliqueNode(specNode->cliquePtr);
    freeSpecNode(specNode->nextSpec);
    free(specNode);
}

void freeHashBucket(HashBucket *hashBucket)
{
    if (hashBucket == NULL)
    {
        return;
    }
    freeSpecNode(hashBucket->specList);
    free(hashBucket);
}

void freeHashTable(HashTable *hashTable)
{
    if (hashTable == NULL)
        return;
    for (int i = 0; i < hashTable->size; i++)
    {
        freeMissMatchVector(hashTable->hashArray[i]);
    }
    for (int i = 0; i < hashTable->size; i++)
    {
        freeHashBucket(hashTable->hashArray[i]);
    }
    free(hashTable->hashArray);
    free(hashTable);
}

void printHashingBenchmarks()
{
    printf("Bucket pointers created: %d\n", bucketsCreated);
    printf("Insertions: %d\n", insertions);
    printf("Collisions: %d\n", collisions);
    printf("Searches in chains: %d\n", searchInChain);
}

HashTable *getMainHTable()
{
    return mainHTable;
}

Vector *getMatchesVector()
{
    return matchesVector;
}

Vector *getMissMatchesVector()
{
    return missMatchesVector;
}