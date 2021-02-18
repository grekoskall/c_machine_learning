#include <stddef.h>
#include "Unity/src/unity.h"

void test_spec();
void test_reading();
void test_hashing();
void test_clique();
void test_vector();
void test_hashing_w();

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{

  test_reading();
  test_spec();
  test_hashing();
  test_clique();
  test_vector();
  test_hashing_w();

  return 0;
}

void printExecTime()
{
}
