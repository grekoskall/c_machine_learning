#include <stddef.h>
#include "Unity/src/unity.h"

#include "../hdr/includes.h"

void test_initVector(void)
{
    Vector *vector = vectorInit();
    TEST_ASSERT(vector->capacity == 500);
    freeVector(vector);
}

void test_vectorPushBack(void)
{
    Vector *vector = vectorInit();
    char *example = createString("kalispera");
    vectorPushBack(vector, example);
    TEST_ASSERT_EQUAL_STRING("kalispera", vectorGet(vector, 0));
    char *example2 = createString("test 2");
    vectorPushBack(vector, example2);
    TEST_ASSERT_EQUAL_STRING("test 2", vectorGet(vector, 1));
    freeVector(vector);
}

void test_csvToVector(void)
{
    Vector *vector = readCsvToVector("./Datasets/stopwords.csv");
    TEST_ASSERT_EQUAL_STRING("cannot", vectorGet(vector, 22));
    freeVector(vector);
}

void test_vector()
{
    UnityBegin("../test/test_vector.c");

    RUN_TEST(test_initVector);
    RUN_TEST(test_vectorPushBack);
    RUN_TEST(test_csvToVector);

    UnityEnd();
}