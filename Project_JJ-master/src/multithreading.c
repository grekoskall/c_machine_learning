#include "../hdr/includes.h"

double *dj;
pthread_mutex_t dj_access;
double *testAccuracy;
pthread_mutex_t acc_access;

static void cleanup_handler(void *arg)
{
    (void)pthread_mutex_unlock(((thread_args_t *)arg)->mt);
    //(void)pthread_mutex_unlock(&dj_access);
}

void *thread_func(void *arg)
{
    pthread_cleanup_push(cleanup_handler, arg);

    pthread_barrier_wait(((thread_args_t *)arg)->barrier);

    while (1)
    {
        //pthread_testcancel();
        Job *job;
        QueueNode *qn;

        pthread_mutex_lock(((thread_args_t *)arg)->mt);
        if (isempty(((thread_args_t *)arg)->q))
        {
            pthread_mutex_unlock(((thread_args_t *)arg)->mt);
            //printf("block on cv: %d\n", ((thread_args_t*)arg)->cv);
            //pthread_cond_signal(((thread_args_t *)arg)->cv);
            pthread_barrier_wait(((thread_args_t *)arg)->barrier);
            break;
        }

        qn = queue_pop(((thread_args_t *)arg)->q);
        pthread_mutex_unlock(((thread_args_t *)arg)->mt);

        job = qn->data;
        job->function_execute(job->any_parameter);

        free(job->any_parameter);
        free(job);
        free(qn);
    }

    pthread_cleanup_pop(false);
    return NULL;
}

JobScheduler *scheduler_init(int no_of_threads)
{
    JobScheduler *scheduler = (JobScheduler *)safe_malloc(sizeof(JobScheduler));
    ec_nzero(pthread_mutex_init(&scheduler->mt, NULL), "failed mutex init");
    //ec_nzero(pthread_cond_init(&scheduler->cv, NULL), "failed cond init");
    ec_nzero(pthread_mutex_init(&scheduler->mtcv, NULL), "failed mutex init");
    ec_nzero(pthread_barrier_init(&scheduler->barrier, NULL, no_of_threads + 1), "failed barrier init");
    scheduler->q = initialize_queue();
    scheduler->threads = no_of_threads;
    scheduler->tids = (pthread_t *)safe_malloc(sizeof(pthread_t) * no_of_threads);
    scheduler->thread_args = (thread_args_t *)safe_malloc(sizeof(thread_args_t));
    scheduler->thread_args->q = scheduler->q;
    //scheduler->thread_args->cv = &(scheduler->cv);
    scheduler->thread_args->mt = &(scheduler->mt);
    scheduler->thread_args->barrier = &(scheduler->barrier);
    for (int i = 0; i < no_of_threads; i++)
    {
        ec_nzero(pthread_create(&scheduler->tids[i], NULL, thread_func, scheduler->thread_args), "failed create");
    }
    return scheduler;
}

void scheduler_submit_job(JobScheduler *sch, Job *j)
{
    QueueNode *nn = (QueueNode *)safe_malloc(sizeof(QueueNode));
    nn->data = j;
    nn->next = NULL;
    //pthread_mutex_lock(&sch->mt);
    add(sch->q, nn);
    //pthread_cond_signal(&sch->cv);
    //pthread_mutex_unlock(&sch->mt);
}

void scheduler_destroy(JobScheduler *sch)
{
    for (int i = 0; i < sch->threads; i++)
    {
        //ec_nzero(pthread_cancel(sch->tids[i]), "failed pthread cancel");
        ec_nzero(pthread_join(sch->tids[i], NULL), "failed pthread join");
    }

    free(sch->thread_args);
    free(sch->tids);
    destroy_queue(sch->q);
    ec_nzero(pthread_barrier_destroy(&sch->barrier), "failed barrier destroy");
    ec_nzero(pthread_mutex_destroy(&sch->mt), "failed mutex destroy");
    ec_nzero(pthread_mutex_destroy(&sch->mtcv), "failed mutex destroy");
    //ec_nzero(pthread_cond_destroy(&sch->cv), "failed cond destroy");

    free(sch);
}

void scheduler_execute_all(JobScheduler *sch)
{
    pthread_barrier_wait(&sch->barrier);
}

void scheduler_wait_finish(JobScheduler *sch)
{
    pthread_barrier_wait(&sch->barrier);
    // pthread_mutex_lock(&sch->mtcv);
    // pthread_cond_wait(&sch->cv, &sch->mtcv);
    // pthread_mutex_unlock(&sch->mtcv);
}

Queue *initialize_queue()
{
    Queue *nq = (Queue *)safe_malloc(sizeof(Queue));
    nq->head = NULL;
    nq->size = 0;
    return nq;
}

void destroy_queue(Queue *q)
{
    if (q == NULL)
        return;
    if (q->head == NULL)
    {
        free(q);
        return;
    }
    QueueNode *temp = q->head;
    QueueNode *next = q->head->next;
    while (next != NULL)
    {
        //free(temp->data);
        free(temp);
        temp = next;
        next = next->next;
    }
    //free(temp->data);
    free(temp);
    free(q);
}

void add(Queue *q, QueueNode *nn)
{
    if (q == NULL)
        return;
    if (q->head == NULL)
    {
        q->head = nn;
        q->size++;
        return;
    }
    QueueNode *temp = q->head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = nn;
    q->size++;
}

int isempty(Queue *q)
{
    if (q == NULL || q->head == NULL)
        return 1;
    return 0;
}

QueueNode *queue_pop(Queue *q)
{
    if (q == NULL || q->head == NULL)
        return NULL;
    QueueNode *nj = (QueueNode *)q->head;
    q->head = q->head->next;
    return nj;
}

void calculate_accuracy(void *param)
{
    int items_start = ((CalculateAccuracy *)param)->place;
    int items_end = items_start + TEST_BATCH_SIZE;
    if (items_end > ((CalculateAccuracy *)param)->pairs->itemsInserted)
        items_end = ((CalculateAccuracy *)param)->pairs->itemsInserted;
    double hits = 0.0;
    double no_hits = 0.0;
    for (int i = items_start; i < items_end; i++)
    {
        if (((CalculateAccuracy *)param)->pairs->items[i] == NULL)
            continue;
        double px = p_logistic_function_full(((CalculateAccuracy *)param)->pairs->items[i], ((CalculateAccuracy *)param)->b);
        double y = ((Observation *)((CalculateAccuracy *)param)->pairs->items[i])->isMatch;
        //printf("==> (+++) ( %f, %f )\n", px, y);
        if (fabs(px - y) <= 0.5)
            hits++;
        else
            no_hits++;
    }

    int position = ((CalculateAccuracy *)param)->i;
    pthread_mutex_lock(&acc_access);
    if (testAccuracy[position] > 0.0)
        testAccuracy[position] = (testAccuracy[position] + (hits) / ((double)hits + no_hits)) / 2.0;
    else
        testAccuracy[position] = (hits) / ((double)hits + no_hits);
    pthread_mutex_unlock(&acc_access);
}

double timeSpentTesting;

double model_testing_testing(HashTable *hash_table, Vector *full_T_pairs, double *b, int _threads, float _threshold, int _batch_size)
{
    pthread_mutex_init(&acc_access, NULL);
    puts("==> Initiating model TESTING ...");
    //clock_t testing_start = clock();
    double accuracy = 0.0;
    float threshold = _threshold;

    int times_inserted;
    if (full_T_pairs->itemsInserted <= _batch_size)
        times_inserted = 1;
    else
        times_inserted = full_T_pairs->itemsInserted / _batch_size + 1;

    int test_acc_arr_size = 0;
    if (_threads >= times_inserted)
        test_acc_arr_size = times_inserted;
    else
        test_acc_arr_size = _threads;
    testAccuracy = (double *)calloc(test_acc_arr_size, sizeof(double));

    while (threshold < 0.5)
    {
        for (int i = 0; i < test_acc_arr_size; i++)
        {
            testAccuracy[i] = 0.0;
        }
        //puts("==> Calculating accuracy ... ");

        JobScheduler *sch = scheduler_init(_threads);

        for (int i = 1; i < times_inserted + 1; i++)

        {
            Job *new_job = (Job *)safe_malloc(sizeof(Job));
            CalculateAccuracy *to_pass = (CalculateAccuracy *)safe_malloc(sizeof(CalculateAccuracy));
            new_job->function_execute = calculate_accuracy;
            to_pass->pairs = full_T_pairs;
            to_pass->b = b;
            if (_threads >= times_inserted)
                to_pass->i = i - 1;
            else
                to_pass->i = (i - 1) % _threads;

            to_pass->place = (i - 1) * TEST_BATCH_SIZE;
            new_job->any_parameter = to_pass;
            scheduler_submit_job(sch, new_job);
        }

        scheduler_execute_all(sch);
        scheduler_wait_finish(sch);
        threshold += THRESHOLD_STEP * THRESHOLD_SLOPE;

        scheduler_destroy(sch);
        accuracy = 0;
        for (int i = 0; i < test_acc_arr_size; i++)
        {
            accuracy += testAccuracy[i];
        }
        accuracy = accuracy / ((double)test_acc_arr_size);
    }

    puts("==> Model testing COMPLETED ...");
    free(testAccuracy);
    testAccuracy = NULL;
    //clock_t testing_end = clock();
    //timeSpentTesting = (double)(testing_end - testing_start) / CLOCKS_PER_SEC;
    //printf("==> [+++] Testing Time [ %f ]\n", timeSpentTesting);
    pthread_mutex_destroy(&acc_access);

    return accuracy;
}

// VALIDATION NOT TESTING
double model_testing(HashTable *hash_table, Vector *full_V_pairs, double *b)
{
    pthread_mutex_init(&acc_access, NULL);
    puts("==> Initiating model VALIDATION ...");
    //clock_t validation_start = clock();
    double accuracy = 0.0;
    float threshold = THRESHOLD_VALUE;

    int times_inserted;
    if (full_V_pairs->itemsInserted <= TEST_BATCH_SIZE)
        times_inserted = 1;
    else
        times_inserted = full_V_pairs->itemsInserted / TEST_BATCH_SIZE + 1;

    int test_acc_arr_size = 0;
    if (TEST_THREAD_NUM >= times_inserted)
        test_acc_arr_size = times_inserted;
    else
        test_acc_arr_size = TEST_THREAD_NUM;
    testAccuracy = (double *)calloc(test_acc_arr_size, sizeof(double));
    for (int i = 0; i < test_acc_arr_size; i++)
    {
        testAccuracy[i] = 0.0;
    }
    while (threshold < 0.5)
    {
        //puts("==> Calculating accuracy ... ");

        for (int i = 0; i < test_acc_arr_size; i++)
        {
            testAccuracy[i] = 0.0;
        }

        JobScheduler *sch = scheduler_init(TEST_THREAD_NUM);
        for (int i = 1; i < times_inserted + 1; i++)
        {
            Job *new_job = (Job *)safe_malloc(sizeof(Job));
            CalculateAccuracy *to_pass = (CalculateAccuracy *)safe_malloc(sizeof(CalculateAccuracy));
            new_job->function_execute = calculate_accuracy;
            to_pass->pairs = full_V_pairs;
            to_pass->b = b;

            if (TEST_THREAD_NUM >= times_inserted)
                to_pass->i = i - 1;
            else
                to_pass->i = (i - 1) % TEST_THREAD_NUM;

            to_pass->place = (i - 1) * TEST_BATCH_SIZE;
            new_job->any_parameter = to_pass;
            scheduler_submit_job(sch, new_job);
        }
        scheduler_execute_all(sch);
        scheduler_wait_finish(sch);
        threshold += THRESHOLD_STEP * THRESHOLD_SLOPE;

        scheduler_destroy(sch);
        accuracy = 0;
        for (int i = 0; i < test_acc_arr_size; i++)
        {
            accuracy += testAccuracy[i];
        }
        accuracy = accuracy / ((double)test_acc_arr_size);
    }

    puts("==> Model validation COMPLETED ...");
    //clock_t validation_end = clock();
    free(testAccuracy);
    testAccuracy = NULL;
    //timeSpentTesting = (double)(validation_end - validation_start) / CLOCKS_PER_SEC;
    //printf("==> [+++] Validation Time [ %f ]\n", timeSpentTesting);
    pthread_mutex_destroy(&acc_access);
    return accuracy;
}

double timeSpentTraining;

double *train_weights(HashTable *hash_table, HashTable_w *W1, Vector *full_W_pairs)
{
    srand(time(NULL));
    //clock_t train_start = clock();
    puts("==> Initiating model TRAINING ...");
    double *b = weight_array_init(TF_IDF_SIZE * 2); // init array based on tf-idfs size
    //printf("Total pairs in: %ld, Total pairs vectorized: %d\n", W1->itemsInserted, full_W_pairs->itemsInserted);
    float threshold = THRESHOLD_VALUE;
    dj = (double *)calloc(TF_IDF_SIZE * 2, sizeof(double));
    pthread_mutex_init(&dj_access, NULL);
    int finish = 0;
    while (threshold < 0.5 && finish < 3)
    {
        puts("==> Training model weights ...");
        printf("==> Threads that are being used: %d ...\n", THREADS_NUM);

        for (int i = 0; i < TF_IDF_SIZE * 2; i++)
        {
            dj[i] = 0.0;
        }
        b = thrd_model_training_wghts(full_W_pairs, b, THREADS_NUM); // !THREADS

        Observation *new_pair_not_in_W;
        puts("==> Adding new pairs to training set ...");
        int pairs_count = 0;
        int count = 0;
        while (pairs_count < NEW_PAIRS_SIZE)
        {
            new_pair_not_in_W = get_pair_in_x_excl_set(hash_table, W1);
            double px = p_logistic_function_full(new_pair_not_in_W, b);
            new_pair_not_in_W->isMatch = px;
            //printf("+++ ( %f ) --- %f \n", px, 1-threshold);
            //printf("random pair: %s , %s : %f\n", new_pair_not_in_W->leftSpecId, new_pair_not_in_W->rightSpecId, new_pair_not_in_W->isMatch);

            if ((px < threshold) || (px > 1 - threshold))

            {
                //printf("+++ Value passes: %f\n", px);
                count++;
                vectorPushBack(full_W_pairs, new_pair_not_in_W);
                Observation *new_pair = (Observation *)safe_malloc(sizeof(Observation));
                new_pair->leftSpecId = new_pair_not_in_W->leftSpecId;
                new_pair->rightSpecId = new_pair_not_in_W->rightSpecId;
                new_pair->isMatch = new_pair_not_in_W->isMatch;
                new_pair->next = NULL;
                new_pair->left_tf_idf = new_pair_not_in_W->left_tf_idf;
                new_pair->right_tf_idf = new_pair_not_in_W->right_tf_idf;
                //printf("==> Adding new pair: ( %f, %s, %s )\n", new_pair->isMatch, new_pair->leftSpecId, new_pair->rightSpecId);
                addToHashTable_w(W1, new_pair);
            }
            //free(new_pair_not_in_W);
            pairs_count++;
        }
        printf("==> New pairs added: %d ...\n", count);
        puts("==> Resolving transitivity issues between pairs ...");
        resolve_transitivity(hash_table, new_pair_not_in_W, W1); //
        threshold += THRESHOLD_STEP * THRESHOLD_SLOPE;
        finish++;
    }
    free(dj);
    pthread_mutex_destroy(&dj_access);
    //clock_t train_end = clock();
    //timeSpentTraining = (double)(train_end - train_start) / CLOCKS_PER_SEC;
    //printf("==> [+++] Training Time [ %f ]\n", timeSpentTraining);
    puts("==> Model training COMPLETED ...");
    print_positive_set(W1);
    return b;
}

void calculate_dj(void *param)
{
    int items_start = ((CalculateDJ *)param)->place;
    int items_end = items_start + BATCH_SIZE;
    if (items_end > ((CalculateDJ *)param)->pairs->itemsInserted)
        items_end = ((CalculateDJ *)param)->pairs->itemsInserted;
    //printf("Start: %d, End: %d, Inserted: %d\n", items_start, items_end, ((CalculateDJ*)param)->pairs->itemsInserted);
    for (int j = 0; j < TF_IDF_SIZE * 2; j++)
    {
        double sum = 0.0;
        for (int i = items_start; i < items_end; i++)
        {
            if ((Observation *)((CalculateDJ *)param)->pairs->items == NULL || ((Observation *)((CalculateDJ *)param)->pairs->items[i]) == NULL)
            {
                break;
            }
            if (j < TF_IDF_SIZE)
            {
                if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->left_tf_idf->itemsInserted <= j)
                    break;
                if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->left_tf_idf->items[j] == NULL)

                    continue;
                if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->left_tf_idf->items[i] == NULL)

                    continue;
            }
            else
            {
                if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->right_tf_idf->itemsInserted <= j - TF_IDF_SIZE)
                    break;
                if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->right_tf_idf->items[j] == NULL)

                    continue;
                if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->right_tf_idf->items[i] == NULL)

                    continue;
            }
            double px = p_logistic_function_full(((CalculateDJ *)param)->pairs->items[i], ((CalculateDJ *)param)->b);
            //printf("(++++++++) IN WEIGHT: %f\n", px);
            double px_y = px - ((Observation *)((CalculateDJ *)param)->pairs->items[i])->isMatch;
            double in_sum = 0.0;
            if (j < TF_IDF_SIZE)
            {

                // if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->left_tf_idf->items[i] == NULL)
                //   break;
                //else
                in_sum = px_y * ((tf_idfInfo *)((Observation *)((CalculateDJ *)param)->pairs->items[i])->left_tf_idf->items[j])->tf_idfValue;
            }

            else
            {

                ///if (((Observation *)((CalculateDJ *)param)->pairs->items[i])->right_tf_idf->items[i] == NULL)
                //  break;
                // else
                in_sum = px_y * ((tf_idfInfo *)((Observation *)((CalculateDJ *)param)->pairs->items[i])->right_tf_idf->items[j - TF_IDF_SIZE])->tf_idfValue;
            }
            sum += in_sum;
        }

        sum = sum / ((double)BATCH_SIZE);

        pthread_mutex_lock(&dj_access);
        dj[j] += sum;
        pthread_mutex_unlock(&dj_access);
    }
}

double *thrd_model_training_wghts(Vector *pairs, double *b, int threads)
{

    int times_inserted, count = 1;

    if (pairs->itemsInserted <= BATCH_SIZE)
        times_inserted = 1;
    else
        times_inserted = pairs->itemsInserted / BATCH_SIZE + 1;
    //printf("Times inserted : %d\n", times_inserted);

    //int flag = 0;
    while (count <= WEIGHT_TR_NUM)
    {
        for (int i = 0; i < TF_IDF_SIZE * 2; i++)
        {
            dj[i] = 0.0;
        }

        if (count % 50 == 0)
            printf("==> Training weights times %d ...\n", count);

        JobScheduler *sch = scheduler_init(threads);

        for (int i = 1; i < times_inserted + 1; i++)

        {
            Job *new_job = (Job *)safe_malloc(sizeof(Job));
            CalculateDJ *to_pass = (CalculateDJ *)safe_malloc(sizeof(CalculateDJ));

            new_job->function_execute = calculate_dj;
            to_pass->place = (i - 1) * BATCH_SIZE;
            to_pass->pairs = pairs;
            to_pass->b = b;
            new_job->any_parameter = to_pass;
            scheduler_submit_job(sch, new_job);
        }
        //puts("==> Executing all jobs ...");
        scheduler_execute_all(sch);

        //puts("==> Waiting to finish ...");
        scheduler_wait_finish(sch);
        //puts("==> Calculating final dJ ...");

        for (int i = 0; i < TF_IDF_SIZE * 2; i++)
        {
            dj[i] = dj[i] / ((double)times_inserted);
            b[i] = b[i] - LEARNING_RATE * dj[i];
        }
        //puts("==> Executing all jobs COMPLETED ...");
        count++;
        //flag = 1;
        /*for ( int i = 0; i < TF_IDF_SIZE*2 ; i++ ) {
            if ( -LEARNING_RATE*dj[i] > E_VALUE ) {
                //flag = 0;
                break;
            }
        }*/

        scheduler_destroy(sch);
    }
    puts("==> Training model weights COMPLETED ...");

    return b;
}

void print_positive_set(HashTable_w *W1)
{
    if (W1 == NULL || W1->hashArray == NULL || W1->size == 0)
        return;
    FILE *fptr = open_file("./output/model_positive.txt");
    puts("==> Printing Positive Matches ... ");
    for (int i = 0; i < W1->size; i++)
    {
        if (W1->hashArray[i] == NULL)
            continue;
        Observation *list = W1->hashArray[i]->observationsList;
        while (list != NULL)
        {
            if (list->isMatch >= 0.5)
            {
                if (fptr != NULL)
                    fprintf(fptr, " %s, %s, %f\n", list->leftSpecId, list->rightSpecId, list->isMatch);
            }
            list = list->next;
        }
    }
    fclose(fptr);
    return;
}

void resolve_transitivity(HashTable *hash_table, Observation *pair, HashTable_w *W1)
{
    return;
}

double p_logistic_function_full(Observation *pair, double *weights_array)
{
    double f_x = 0.0;
    for (int i = 0; i < TF_IDF_SIZE; i++)
    {
        if (pair->left_tf_idf->itemsInserted <= i)
            break;
        if (pair->left_tf_idf->items[i] == NULL)
            continue;
        double weight = weights_array[i];
        double x_value = ((tf_idfInfo *)pair->left_tf_idf->items[i])->tf_idfValue;
        f_x += weight * x_value;
        //printf ( "%f\n", ((tf_idfInfo*)pair->left_tf_idf->items[i])->tf_idfValue);
    }
    for (int i = TF_IDF_SIZE; i < TF_IDF_SIZE * 2; i++)
    {
        if (pair->right_tf_idf->itemsInserted <= i - TF_IDF_SIZE)
            break;
        if (pair->right_tf_idf->items[i - TF_IDF_SIZE] == NULL)
            continue;
        //printf ( "%f\n", ((tf_idfInfo*)pair->right_tf_idf->items[i-TF_IDF_SIZE])->tf_idfValue);
        double weight = weights_array[i];
        double x_value = ((tf_idfInfo *)pair->right_tf_idf->items[i - TF_IDF_SIZE])->tf_idfValue;
        f_x += weight * x_value;
    }

    double e_x = exp(-f_x);
    double p_x = 1.0 / ((double)(1.0 + e_x));

    return p_x;
}

double *weight_array_init(int size)
{
    double *new_array = (double *)safe_calloc(size, sizeof(double));
    for (int i = 0; i < size; i++)
    {
        new_array[i] = WEIGHTS_START_VAL;
    }
    return new_array;
}

Observation *get_pair_in_x_excl_set(HashTable *hash_table, HashTable_w *W1)
{
    int count = 0;

    SpecInfo *randm_spc1;
    SpecInfo *randm_spc2;
    Observation *test_pair = get_first_pair_w(W1);
    if (test_pair == NULL)
    {
        printf("Error: Unable to find pairs W1 is not initialized.");
        exit(1);
    }
    while (test_pair != NULL)
    {
        randm_spc1 = hashtable_find_random_spec(hash_table);
        randm_spc2 = hashtable_find_random_spec(hash_table);
        while (strcmp(randm_spc1->specId, randm_spc2->specId) == 0)
        {
            if (count % 3 == 0 || count % 7 == 0)
            {
                randm_spc2 = hashtable_find_random_spec(hash_table);
            }
            else
            {
                randm_spc1 = hashtable_find_random_spec(hash_table);
            }
            count++;
        }
        test_pair = searchHashTable_w(W1, randm_spc1->specId, randm_spc2->specId);
        if (test_pair == NULL)
        {
            test_pair = searchHashTable_w(W1, randm_spc2->specId, randm_spc1->specId);
        }
        if (count == 50000)
        {
            printf("Error: Unable to find pairs.");
            exit(1);
        }
    }

    Observation *new_pair = (Observation *)safe_malloc(sizeof(Observation));
    new_pair->leftSpecId = randm_spc1->specId;
    new_pair->left_tf_idf = randm_spc1->tfidf_vector;
    new_pair->rightSpecId = randm_spc2->specId;
    new_pair->right_tf_idf = randm_spc2->tfidf_vector;
    new_pair->isMatch = 0.0;
    new_pair->next = NULL;

    return new_pair;
}

Observation *get_first_pair_w(HashTable_w *W1)
{
    if (W1 == NULL || W1->hashArray == NULL || W1->size == 0)
        return NULL;
    for (int i = 0; i < W1->size; i++)
    {
        if (W1->hashArray[i] == NULL)
            continue;
        if (W1->hashArray[i]->observationsList != NULL)
        {
            return W1->hashArray[i]->observationsList;
        }
    }
    return NULL;
}

SpecInfo *hashtable_find_random_spec(HashTable *hash_table)
{
    int randm_pos = rand() % hash_table->size;

    while (hash_table->hashArray[randm_pos] == NULL || hash_table->hashArray[randm_pos]->specList == NULL)
    {
        randm_pos = rand() % hash_table->size;
    }

    return hash_table->hashArray[randm_pos]->specList->cliquePtr->specInfo;
}

void validate_model(HashTable_w *V, Vector *weights)
{
    return;
}

Vector *vectorize_all_pairs(HashTable *hash_table, HashTable_w *W1)
{
    Vector *full_batch = vectorInit();

    int hash_buckets = W1->size;
    for (int i = 0; i < hash_buckets; i++)
    {
        if (W1->hashArray[i] == NULL)
            continue;
        Observation *pair_list = W1->hashArray[i]->observationsList;
        while (pair_list != NULL)
        {
            char *left_id = pair_list->leftSpecId;
            char *right_id = pair_list->rightSpecId;
            int is_match = pair_list->isMatch;

            SpecNode *spec_node_l = searchHashTable(hash_table, left_id);
            SpecNode *spec_node_r = searchHashTable(hash_table, right_id);

            Vector *left_vector = get_spec_tf_idf_vector(spec_node_l);
            Vector *right_vector = get_spec_tf_idf_vector(spec_node_r);

            Observation *new_observation = (Observation *)safe_malloc(sizeof(Observation));
            new_observation->leftSpecId = left_id;
            new_observation->left_tf_idf = left_vector;
            new_observation->rightSpecId = right_id;
            new_observation->right_tf_idf = right_vector;
            new_observation->isMatch = is_match;

            vectorPushBack(full_batch, new_observation);

            pair_list = pair_list->next;
        }
    }
    return full_batch;
}

Vector *get_spec_tf_idf_vector(SpecNode *spec_node)
{
    if (spec_node == NULL)
        return NULL;
    return spec_node->cliquePtr->specInfo->tfidf_vector;
}