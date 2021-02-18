#include "../hdr/includes.h"

void modelEvaluation(HashTable *hashTable, Vector *evaluationPairsVector, double *w_array)
{
    FILE *fptr_eval = open_file("./output/model_validation.txt");

    double passCounter = 0.0;
    int pairscount = vectorItemsCount(evaluationPairsVector);
    /**
     * VALIDATE 20% OF DATASET W
     * */
    for (int i = 0; i < pairscount; i++)
    {
        char *leftSpecId = ((PairInfo *)evaluationPairsVector->items[i])->leftSpecId;
        char *rightSpecId = ((PairInfo *)evaluationPairsVector->items[i])->rightSpecId;
        int isMatch = ((PairInfo *)evaluationPairsVector->items[i])->isMatch;

        double *tf_idfSum = calcTF_IDFsum(findTF_IDFvector(hashTable, leftSpecId), findTF_IDFvector(hashTable, rightSpecId));

        double p_x = p_logistic_function(f_linear_functionV2(TF_IDF_SIZE, tf_idfSum, w_array));

        if (fabs(1.0 - p_x) <= fabs(0.0 - p_x))
        {
            if (isMatch == 1)
            {
                passCounter++;
                printf("%s , %s, %d : PASS [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
                fprintf(fptr_eval, "%s , %s, %d : PASS [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
            }
            else
            {
                printf("%s , %s, %d : FAIL [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
                fprintf(fptr_eval, "%s , %s, %d : FAIL [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
            }
        }
        else
        {
            if (isMatch == 0)
            {
                passCounter++;
                printf("%s , %s, %d : PASS [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
                fprintf(fptr_eval, "%s , %s, %d : PASS [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
            }
            else
            {
                printf("%s , %s, %d : FAIL [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
                fprintf(fptr_eval, "%s , %s, %d : FAIL [score: %f]\n", leftSpecId, rightSpecId, ((PairInfo *)evaluationPairsVector->items[i])->isMatch, p_x);
            }
        }
    }
    int allPairsFromCliquesCounter = 0;
    /**
     * VALIDATE COMPUTED MATCHES FROM THE CLIQUES
     * */
    FILE *stream = fopen("./output/matches.txt", "r");
    if (stream == NULL)
    {
        printf("Could not create %s\n", "./output/matches.txt");
        exit(EXIT_FAILURE);
    }
    char line[MAXLINE], *leftSpecId, *rightSpecId;
    while (fgets(line, MAXLINE, stream))
    {
        allPairsFromCliquesCounter++;
        char *tmp1 = strdup(line), *tmp2 = strdup(line);
        leftSpecId = getField(tmp1, 1);
        rightSpecId = getField(tmp2, 2);
        double *tf_idfSum = calcTF_IDFsum(findTF_IDFvector(hashTable, leftSpecId), findTF_IDFvector(hashTable, rightSpecId));
        double p_x = p_logistic_function(f_linear_functionV2(TF_IDF_SIZE, tf_idfSum, w_array));
        if (fabs(1.0 - p_x) <= fabs(0.0 - p_x))
        {
            passCounter++;
            printf("%s , %s, 1 : PASS [score: %f]\n", leftSpecId, rightSpecId, p_x);
            fprintf(fptr_eval, "%s , %s, 1 : PASS [score: %f]\n", leftSpecId, rightSpecId, p_x);
        }
        else
        {
            printf("%s , %s, 1 : FAIL [score: %f]\n", leftSpecId, rightSpecId, p_x);
            fprintf(fptr_eval, "%s , %s, 1 : FAIL [score: %f]\n", leftSpecId, rightSpecId, p_x);
        }

        free(tmp1);
        free(tmp2);
    }

    fclose(stream);
    /**
     * VALIDATE COMPUTED MISS-MATCHES FROM THE CLIQUES
     * */
    stream = fopen("./output/miss_matches.txt", "r");
    if (stream == NULL)
    {
        printf("Could not create %s\n", "./output/miss_matches.txt");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, MAXLINE, stream))
    {
        allPairsFromCliquesCounter++;
        char *tmp1 = strdup(line), *tmp2 = strdup(line);
        leftSpecId = getField(tmp1, 1);
        rightSpecId = getField(tmp2, 2);
        double *tf_idfSum = calcTF_IDFsum(findTF_IDFvector(hashTable, leftSpecId), findTF_IDFvector(hashTable, rightSpecId));
        double p_x = p_logistic_function(f_linear_functionV2(TF_IDF_SIZE, tf_idfSum, w_array));
        if (fabs(1.0 - p_x) > fabs(0.0 - p_x))
        {
            passCounter++;
            printf("%s , %s, 0 : PASS [score: %f]\n", leftSpecId, rightSpecId, p_x);
            fprintf(fptr_eval, "%s , %s, 0 : PASS [score: %f]\n", leftSpecId, rightSpecId, p_x);
        }
        else
        {
            printf("%s , %s, 0 : FAIL [score: %f]\n", leftSpecId, rightSpecId, p_x);
            fprintf(fptr_eval, "%s , %s, 0 : FAIL [score: %f]\n", leftSpecId, rightSpecId, p_x);
        }

        free(tmp1);
        free(tmp2);
    }

    fclose(stream);
    /////////////////////////

    printf("SUCCESS RATE: %f%%\n", passCounter / (double)(pairscount + allPairsFromCliquesCounter) * 100.0);
    fprintf(fptr_eval, "SUCCESS RATE: %f%%\n", passCounter / (double)(pairscount + allPairsFromCliquesCounter) * 100.0);

    fclose(fptr_eval);
}
