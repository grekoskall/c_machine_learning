#include "../hdr/includes.h"

void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);

    if (!ptr && (size > 0))
    {
        perror("malloc failed!");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void *safe_calloc(long elementsNum, size_t size)
{
    void *ptr = calloc(elementsNum, size);

    if (ptr == NULL || size <= 0 || elementsNum <= 0)
    {
        perror("calloc failed!");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void *safe_realloc(void *ptr, size_t size)
{
    //printf("size: %ld\n", size);
    void *ptrNew = realloc(ptr, size);
    if (ptrNew == NULL)
    {
        perror("realloc failed!");
        exit(EXIT_FAILURE);
    }

    return ptrNew;
}

char *createString(char *string)
{
    char *newString = safe_malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(newString, string);
    return newString;
}

int same_string(char *string1, char *string2)
{
    if (strcmp(string1, string2) == 0)
        return 1;
    else
        return 0;
}

int validArgs(int argc, char *argv[])
{

    if (argc == 5)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void getArgs(char **datasetX, char **datasetY, char *argv[])
{

    for (int i = 1; i < 5; i = i + 2)
    {
        if (!strcmp(argv[i], "-x"))
        {
            *datasetX = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-w"))
        {
            *datasetY = argv[i + 1];
        }
    }
}

FILE *open_file(char *fileName)
{
    FILE *fptr = fopen(fileName, "w");
    if (fptr == NULL)
    {
        printf("Could not open file");
    }
    return fptr;
}

void printInsights()
{
    puts(" ");
    puts("|========== Insights ==========|");
    puts(" ");
    //printHashingBenchmarks();
    printMatchesCount();
    printMissMatchesCount();
    //printVisitedSpecNodesCount();
    //printExecTime();
    puts(" ");
    puts("|==============================|");
}
