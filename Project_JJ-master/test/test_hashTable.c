#include <stddef.h>
#include "Unity/src/unity.h"

#include "../hdr/includes.h"

void test_initHtable(void)
{
    HashTable *htable = initHashTable(15000);
    TEST_ASSERT_NOT_NULL(htable);
    freeHashTable(htable);
}
void test_initSpecNode(void)
{
    SpecNode *specNode = initSpecNode();
    TEST_ASSERT_NULL(specNode->cliquePtr);
    TEST_ASSERT_NULL(specNode->nextSpec);
    TEST_ASSERT_NOT_NULL(specNode);
    freeSpecNode(specNode);
}

void test_addAndSearchChain()
{
    HashTable *hashTable = initHashTable(1000);
    SpecInfo *specInfo = initSpecInfo("www.ebay.com", "5200.json", "test");
    addToHashTable(hashTable, specInfo);
    SpecInfo *specInfo2 = initSpecInfo("www.tp.gr", "327.json", "test");
    addToHashTable(hashTable, specInfo2);
    SpecNode *specNode = searchHashTable(hashTable, "www.tp.gr//327");
    TEST_ASSERT_EQUAL_STRING("www.tp.gr//327", specNode->cliquePtr->specInfo->specId);
    freeHashTable(hashTable);
}

void test_closestPrime(void)
{
    TEST_ASSERT(closestPrime(39000) == 43627);
}

void test_hashFunction(void)
{
    TEST_ASSERT_EQUAL(94027030675556, hashFunction("kalispera"));
}

void test_hashing()
{
    UnityBegin("../test/test_hashTable.c");

    RUN_TEST(test_initHtable);
    RUN_TEST(test_closestPrime);
    RUN_TEST(test_hashFunction);
    RUN_TEST(test_initHtable);
    RUN_TEST(test_initSpecNode);
    RUN_TEST(test_addAndSearchChain);

    UnityEnd();
}
