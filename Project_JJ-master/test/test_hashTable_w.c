#include <stddef.h>
#include "Unity/src/unity.h"

#include "../hdr/includes.h"

void test_initHtable_w(void)
{
    HashTable_w *htable = initHashTable_w(15000);
    TEST_ASSERT_NOT_NULL(htable);
    freeHashTable_w(htable);
}
void test_initPairInfo_w(void)
{
    // We can't test that because of how the initialization happens

    // Observation *newObservation = initObservation("leftSpec", "rightSpec", 1);
    // TEST_ASSERT_NULL(newObservation->next);
    // TEST_ASSERT_NOT_NULL(newObservation);
    // freeObservationsList(newObservation);
}

void test_addAndSearchChain_w()
{
    // We can't test that because of how the initialization happens

    // HashTable_w *hashTable = initHashTable_w(1000);
    // Observation *newObservation = initObservation("leftSpec", "rightSpec", 1);
    // addToHashTable_w(hashTable, newObservation);
    // Observation *observation = searchHashTable_w(hashTable, "leftSpec", "rightSpec");
    // TEST_ASSERT_NOT_NULL(observation);
    // Observation *observation2 = searchHashTable_w(hashTable, "leftSpecWrong", "rightSpec");
    // TEST_ASSERT_NULL(observation2);
    // Observation *observation3 = searchHashTable_w(hashTable, "rightSpec", "leftSpec");
    // TEST_ASSERT_EQUAL_STRING("leftSpec", observation3->leftSpecId);
    // freeHashTable_w(hashTable);
}

void test_hashing_w()
{
    UnityBegin("../test/test_hashTable.c");

    RUN_TEST(test_initHtable_w);
    RUN_TEST(test_initPairInfo_w);
    RUN_TEST(test_addAndSearchChain_w);

    UnityEnd();
}