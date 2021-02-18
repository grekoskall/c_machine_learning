#ifndef _MULTI_THRD_
#define _MULTI_THRD_

#include "../hdr/hash.h"
#include "../hdr/hash_Wplus.h"
#include "../hdr/vectorImpl.h"

#define TEST_THREAD_NUM 10
#define TEST_BATCH_SIZE 1000
#define LEARNING_RATE 0.7

#define THREADS_NUM 20
#define BATCH_SIZE 500
#define NEW_PAIRS_SIZE 10000
#define THRESHOLD_VALUE 0.01
#define THRESHOLD_STEP 0.01

#define THRESHOLD_SLOPE 1
#define WEIGHTS_START_VAL 0.15
#define TF_IDF_SIZE 1000
#define E_VALUE 0.0000001
#define WEIGHT_TR_NUM 200

#define ec_nzero(call, msg) \
    {                       \
        if ((call) < 0)     \
        {                   \
            perror(msg);    \
            exit(1);        \
        }                   \
    }

void *thread_func(void *arg);

typedef struct QueueNode QueueNode;
typedef struct Queue Queue;
typedef struct JobScheduler JobScheduler;
typedef struct Job Job;
typedef struct thread_args thread_args_t;

typedef struct Observation Observation;
typedef struct CalculateDJ CalculateDJ;
typedef struct CalculateAccuracy CalculateAccuracy;

struct JobScheduler
{
    int threads;
    Queue *q;
    pthread_t *tids;
    pthread_mutex_t mtcv; // mutex for finish
    //pthread_cond_t cv;         // threads have finished
    pthread_mutex_t mt;        // queue lock mutex
    pthread_barrier_t barrier; // thread barrier for simultaneous execution
    thread_args_t *thread_args;
};

struct thread_args
{
    pthread_mutex_t *mt;
    pthread_mutex_t *mtcv;
    //pthread_cond_t *cv;
    pthread_barrier_t *barrier;
    Queue *q;
};

struct Job
{
    void (*function_execute)(void *p); // can point to any function (task) with parameters or NULL
    void *any_parameter;
};

struct CalculateDJ
{
    Vector *pairs;
    double *b;
    int place;
};

struct CalculateAccuracy
{
    Vector *pairs;
    double *b;
    int place;
    int i;
};

struct Queue
{
    QueueNode *head;
    int size;
};

struct QueueNode
{
    void *data; // can point to any data type, in this case we want an initialized Job pointer passed to it
    QueueNode *next;
};

/* Job Scheduler Operations */
JobScheduler *scheduler_init(int no_of_threads);
void scheduler_submit_job(JobScheduler *sch, Job *j);
void scheduler_destroy(JobScheduler *sch);
void scheduler_execute_all(JobScheduler *sch);
void scheduler_wait_finish(JobScheduler *sch);

/* queue operations */
Queue *initialize_queue();
void destroy_queue(Queue *q);
void add(Queue *q, QueueNode *nn);
int isempty(Queue *q);
QueueNode *queue_pop(Queue *q); // !Used with queue mutex locked

/**
 * @brief Train the model using all pairs of json files specified in datasetX_path.
 * @param hash_table The hash_table that holds the specs.
 * @param W1 The training set.
 * @param full_W_pairs The training set vectorized with Observations.
 * @returns The vector with the weights calculated by the training.
 */
double *train_weights(HashTable *hash_table, HashTable_w *W1, Vector *full_W_pairs);

/**
 * @brief Validate the model using pairs in V, given weights and print the resulting accuracy.
 * @param V The validation set.
 * @param weights The weights vector that has been calculated by training.
 */
void validate_model(HashTable_w *V, Vector *weights);

/* Model Training with threads */

/**
 * @brief Given the hash table with the specs tf-idf vector and the hash table with the pairs of specs, create a vector with the Observations in pairs.
 * @param hash_table The hash table with all the specs and their tf-idf Vector.
 * @param W1 The hash table with all pairs of specs and their relationship 1 or 0 (is or is not match).
 * @returns A vector with Observation structs for each pair in W1.
 */
Vector *vectorize_all_pairs(HashTable *hash_table, HashTable_w *W1);

/**
 * @brief Given a SpecNode return its tf_idf vector.
 * @param spec_node The SpecNode that will be used.
 * @returns The tf_idf vector of the SpecNode.
 */
Vector *get_spec_tf_idf_vector(SpecNode *spec_node);

/**
 * @brief Given a HashTable with specs, find a pair that doesn't exist in HashTable_w, make a new Observation and return it.
 * @param hash_table The HashTable with individual specs.
 * @param W1 The HashTable_w with pairs of specIDs.
 * @returns An Observation for a pair not in W1. Initial is_match value is -1.
 */
Observation *get_pair_in_x_excl_set(HashTable *hash_table, HashTable_w *W1);

/**
 * @brief Initialize the array that will hold weight values. All weights start with the same default value.
 * @param size The size of the array.
 * @returns The initialized array.
 */
double *weight_array_init(int size);

//double *thrd_model_training_wghts(HashTable *hash_table, HashTable_w *W1, double *b);

/**
 * @brief Calculate, update and return the estimated value for the pair based on the weights given.
 * @param pair The Observation that will be used.
 * @param weights_array The array with the weights for the Observation.
 * @returns The estimated value of matching or not matching.
 */
double p_logistic_function_full(Observation *pair, double *weights_array);

/**
 * @brief Resovle Transitivity issues.
 */
void resolve_transitivity(HashTable *hash_table, Observation *pair, HashTable_w *W1);

/**
 * @brief Given a hash table with the pairs, print all pairs that are estimated to match.
 * @param W1 The hash table with the pairs and their matching estimation.
 */
void print_positive_set(HashTable_w *W1);

/**
 * @brief Train the model given the set in Vector and with certain weights. The stochastic descend part is implemented with threads.
 * @param pairs The vector containing Observation pairs of the training set.
 * @param b The array with the weights.
 * @param threads The number of threads used.
 * @returns The array with the new weights calculated with stochastic descend.
 */
double *thrd_model_training_wghts(Vector *pairs, double *b, int threads);

SpecInfo *hashtable_find_random_spec(HashTable *hash_table);

Observation *get_first_pair_w(HashTable_w *W1);

double model_testing(HashTable *hash_table, Vector *full_V_pairs, double *b);

double model_testing_testing(HashTable *hash_table, Vector *full_T_pairs, double *b, int threads, float threshold, int batch_size);

#endif