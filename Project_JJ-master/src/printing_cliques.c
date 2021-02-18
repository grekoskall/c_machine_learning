#include "../hdr/includes.h"

void print_cliques(HashTable *hashTable) {
  printf("=========================================\n");
  printf("|||\t\tPRINTING MATCHES\n");

  if (hashTable == NULL ) {
    printf("%s\n", "UNINITIALIZED Hashtable");
    return;
  }

  if (hashTable->size == 0 || hashTable->hashArray == NULL ) {
    printf("%s\n", "EMPTY Hashtable");
    return;
  }

  int i;
  for ( i = 0; i < hashTable->size; i++ ) {
    print_cliques_bucket(hashTable->hashArray[i]);
  }
  return;
}

void print_cliques_bucket(HashBucket *hashBucket) {
  if ( hashBucket == NULL ) {
    //printf("%s\n", "UNINITIALIZED Hashbucket");
    return;
  }
  if ( hashBucket->specList == NULL ) {
    printf("%s\n", "EMPTY HashBucket");
    return;
  }

  SpecNode *temp = hashBucket->specList;
  while ( temp != NULL ) {
    print_cliques_node(temp);
    temp = temp->nextSpec;
  }

  return;
}


void print_cliques_node(SpecNode *specNode) {
  if ( specNode == NULL ) {
    printf("%s\n", "UNINITIALIZED Specnode");
    return;
  }
  if ( specNode->cliquePtr == NULL ) {
    printf("%s\n", "EMPTY Specnode");
    return;
  }

  CliqueNode *temp = specNode->cliquePtr->next;
  CliqueNode *next = specNode->cliquePtr;
  printf("%s\t%s\n", temp->specInfo->specId, next->specInfo->specId);
  if ( temp->isPrinted == 0 ) {
    //printf("|||\tSPEC:\t%s\tMATCHED WITH\n", temp->specInfo->specId);
    //printf("|||%s\n", temp->specInfo->specId);
    printf("||| >> NEW CLIQUE WITH THE FOLLOWING MATCHING SPECS:\n");
    print_spec_id(temp->specInfo);
    //printf("|||\t%s\n", temp->specInfo->specId);
    temp->isPrinted = 1;
    //temp = temp->prev;
    print_spec_id(next->specInfo);
    //printf("%s\t%s\n", temp->specInfo->specId, next->specInfo->specId);
    /*while ( temp != t_temp ) {
      if ( temp->isPrinted == 1 ) {
        printf("%s\n", "ERRONEOUS CIRCLE CHECK CLIQUES");
        return;
      }
      print_spec_id(temp->specInfo);
      temp->isPrinted = 1;
      temp = temp->prev;
    }*/
    printf("||||||||||||||||||||||||||||||||||||||||||||||||||\n");
  }
  return;
}


void print_spec_id(SpecInfo *specInfo) {
  if ( specInfo == NULL ) {
    printf("%s\n", "UNINITIALIZED Specinfo");
    return;
  }
  if ( specInfo->specId == NULL ) {
    printf("%s\n", "EMPTY Specinfo");
    return;
  }

  printf("|||\t%s\n", specInfo->specId);
  return;
}
