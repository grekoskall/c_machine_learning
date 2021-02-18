#include <stddef.h>
#include "Unity/src/unity.h"

#include "../hdr/includes.h"

void test_initCliqueNode(void)
{
    CliqueNode *cliqueNode = initCliqueNode();
    TEST_ASSERT_NOT_NULL(cliqueNode);
    TEST_ASSERT_NULL(cliqueNode->specInfo);
    TEST_ASSERT_EQUAL_PTR(cliqueNode, cliqueNode->next);
    TEST_ASSERT_EQUAL_PTR(cliqueNode, cliqueNode->prev);
    freeCliqueNode(cliqueNode);
}

void test_searchCliqueNode(void)
{
    HashTable *hashTable = initHashTable(1000);
    SpecInfo *specInfo = initSpecInfo("www.ebay.com", "5200.json", "test");
    addToHashTable(hashTable, specInfo);
    SpecInfo *specInfo2 = initSpecInfo("www.tp.gr", "327.json", "test");
    addToHashTable(hashTable, specInfo2);
    CliqueNode *cliqueNode = getCliqueNode("www.tp.gr//327", hashTable);
    TEST_ASSERT_EQUAL_STRING("www.tp.gr//327", cliqueNode->specInfo->specId);
    freeHashTable(hashTable);
}

void test_alreadyInSameClique()
{
    HashTable *hashTable = initHashTable(count_datafiles("./Datasets/2013_camera_specs/"));
    read_from_dir("./Datasets/2013_camera_specs/", hashTable);
    readDictionary("./test_files/test_csv.csv", hashTable);
    CliqueNode *leftcliqueNode = getCliqueNode("www.shopbot.com.au//331", hashTable);
    CliqueNode *rightcliqueNode = getCliqueNode("www.mypriceindia.com//155", hashTable);
    TEST_ASSERT(alreadyInSameClique(leftcliqueNode, rightcliqueNode) == 1);
    leftcliqueNode = getCliqueNode("www.shopbot.com.au//331", hashTable);
    rightcliqueNode = getCliqueNode("www.mypriceindia.commmm//155", hashTable);
    TEST_ASSERT(alreadyInSameClique(leftcliqueNode, rightcliqueNode) == 0);
    leftcliqueNode = getCliqueNode("www.shopbot.com.au//331", hashTable);
    rightcliqueNode = getCliqueNode("www.ebay.com//45946", hashTable);
    TEST_ASSERT(alreadyInSameClique(leftcliqueNode, rightcliqueNode) == 0);

    freeHashTable(hashTable);
}

void test_cliqueAdjustments(void)
{
    HashTable *hashTable = initHashTable(count_datafiles("./Datasets/2013_camera_specs/"));
    read_from_dir("./Datasets/2013_camera_specs/", hashTable);
    readDictionary("./test_files/test_csv.csv", hashTable);
    CliqueNode *leftcliqueNode = getCliqueNode("www.shopbot.com.au//331", hashTable);
    CliqueNode *rightcliqueNode = getCliqueNode("www.ebay.com//58588", hashTable);
    makeCliqueAdjustmnets(leftcliqueNode, rightcliqueNode);
    leftcliqueNode = getCliqueNode("www.mypriceindia.com//155", hashTable);
    TEST_ASSERT(alreadyInSameClique(leftcliqueNode, rightcliqueNode) == 1);

    freeHashTable(hashTable);
}

void test_removeFromClique(void)
{
    HashTable *hashTable = initHashTable(count_datafiles("./Datasets/2013_camera_specs/"));
    read_from_dir("./Datasets/2013_camera_specs/", hashTable);
    readDictionary("./test_files/test_csv.csv", hashTable);
    CliqueNode *leftcliqueNode = getCliqueNode("www.shopbot.com.au//331", hashTable);
    CliqueNode *rightcliqueNode = getCliqueNode("www.mypriceindia.com//155", hashTable);
    removeFromClique(rightcliqueNode);
    TEST_ASSERT(alreadyInSameClique(leftcliqueNode, rightcliqueNode) == 0);

    freeHashTable(hashTable);
}

void test_resetAllPrintedStatus(void)
{
    HashTable *hashTable = initHashTable(count_datafiles("./Datasets/2013_camera_specs/"));
    read_from_dir("./Datasets/2013_camera_specs/", hashTable);
    readDictionary("./test_files/test_csv.csv", hashTable);
    CliqueNode *cliqueNode = getCliqueNode("www.shopbot.com.au//331", hashTable);
    cliqueNode->isPrinted = 1;
    TEST_ASSERT(cliqueNode->isPrinted == 1);
    resetAllPrintedStatus(hashTable);
    TEST_ASSERT(cliqueNode->isPrinted == 0);

    freeHashTable(hashTable);
}

void test_clique()
{
    UnityBegin("../test/test_clique.c");

    RUN_TEST(test_initCliqueNode);
    RUN_TEST(test_searchCliqueNode);
    RUN_TEST(test_alreadyInSameClique);
    RUN_TEST(test_cliqueAdjustments);
    //RUN_TEST(test_removeFromClique);
    RUN_TEST(test_resetAllPrintedStatus);

    UnityEnd();
}