#ifndef _VECTOR_H
#define _VECTOR_H

#define VECTOR_INIT_SIZE 500

typedef struct Vector Vector;

struct Vector
{
    void **items;
    int capacity;
    int itemsInserted;
};

Vector *vectorInit();

int vectorItemsCount(Vector *vector);

void vectorResize(Vector *vector, int capacity);

void vectorPushBack(Vector *vector, void *item);

void vectorSet(Vector *vector, int index, void *item);

void *vectorGet(Vector *vector, int index);

void freeVector(Vector *vector);

void freeVectorWithoutItems(Vector *vector);

#endif