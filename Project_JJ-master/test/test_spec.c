#include <stddef.h>
#include "Unity/src/unity.h"

#include "../hdr/includes.h"

void test_initSpecInfo(void) {
  SpecInfo *new_spec;
  new_spec = initSpecInfo("site", "111.json", "page_title");
  TEST_ASSERT_NOT_NULL(new_spec);
  TEST_ASSERT_EQUAL_STRING("page_title", new_spec->pageTitle);
  TEST_ASSERT_EQUAL_STRING("site//111", new_spec->specId);
  TEST_ASSERT_NULL(new_spec->infoList);
  freeSpecInfo(new_spec);
}

void test_add_newInfo_toSpec(void) {
  SpecInfo *new_spec;
  new_spec = initSpecInfo("site", "111.json", "page_title");
  add_newInfo_toSpec(new_spec, "desc", "info");
  TEST_ASSERT_NOT_NULL(new_spec->infoList);
  TEST_ASSERT_NOT_NULL(new_spec->infoList->head);
  TEST_ASSERT_EQUAL_STRING("desc", new_spec->infoList->head->description);
  TEST_ASSERT_EQUAL_STRING("info", new_spec->infoList->head->content);
  TEST_ASSERT_NULL(new_spec->infoList->head->next);
  freeSpecInfo(new_spec);
}

void test_add_newInfo_toSpec_extended(void) {
  SpecInfo *new_spec;
  new_spec = initSpecInfo("site", "111.json", "page_title");
  add_newInfo_toSpec(new_spec, "desc", "info");
  add_newInfo_toSpec(new_spec, "desc_2", "info_2");
  TEST_ASSERT_NOT_NULL(new_spec->infoList->head->next);
  TEST_ASSERT_EQUAL_STRING("desc_2", new_spec->infoList->head->next->description);
  TEST_ASSERT_EQUAL_STRING("info_2", new_spec->infoList->head->next->content);
  TEST_ASSERT_NULL(new_spec->infoList->head->next->next);
  freeSpecInfo(new_spec);
}

void test_spec(){
  UnityBegin("../test/test_spec.c");

  RUN_TEST(test_initSpecInfo);
  RUN_TEST(test_add_newInfo_toSpec);
  RUN_TEST(test_add_newInfo_toSpec_extended);
  UnityEnd();
}
