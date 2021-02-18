#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>

void *safe_malloc(size_t size);

void *safe_calloc(long elementsNum, size_t objectSize);

void *safe_realloc(void *ptr, size_t size);

char *createString(char *string);

int same_string(char *string1, char *string2);

int validArgs(int argc, char *argv[]);

void getArgs(char **datasetX, char **datasetY, char *argv[]);

FILE *open_file(char *fileName);

void printInsights();

#endif
