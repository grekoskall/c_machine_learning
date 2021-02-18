#include "../hdr/includes.h"

Vector *vectorInit()
{
    Vector *vectorNew = safe_malloc(sizeof(Vector));
    vectorNew->capacity = VECTOR_INIT_SIZE;
    vectorNew->itemsInserted = 0;
    vectorNew->items = safe_calloc(VECTOR_INIT_SIZE, sizeof(void *));

    return vectorNew;
}

int vectorItemsCount(Vector *vector)
{
    if (vector == NULL)
        return 0;

    return vector->itemsInserted;
}

void vectorResize(Vector *vector, int capacity)
{
    if (vector == NULL)
        return;

    vector->items = safe_realloc(vector->items, sizeof(void *) * capacity);
    vector->capacity = capacity;
    //printf("NEW CAP: %d\n", vector->capacity);
}

void vectorPushBack(Vector *vector, void *item)
{
    if (vector == NULL)
        return;

    if (vector->itemsInserted == vector->capacity) // capacity reached, let's double it's size
    {
        //puts("RESIZED-----------");
        vectorResize(vector, vector->capacity * 1.2);
    }
    vector->items[vector->itemsInserted++] = item;
}

void vectorSet(Vector *vector, int index, void *item)
{
    if (vector == NULL || index < 0 || index >= vector->itemsInserted)
        return;

    vector->items[index] = item;
}

void *vectorGet(Vector *vector, int index)
{
    if (vector == NULL || index < 0 || index >= vector->itemsInserted)
        return NULL;

    return vector->items[index];
}

void freeVector(Vector *vector)
{
    if (vector == NULL)
        return;

    for (int i = 0; i < vector->itemsInserted; i++)
    {
        free(vector->items[i]);
    }
    free(vector->items);
    free(vector);
}

void freeVectorWithoutItems(Vector *vector)
{
    if (vector == NULL)
        return;

    free(vector->items);
    free(vector);
}
