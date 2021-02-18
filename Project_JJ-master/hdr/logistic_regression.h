#ifndef _LOG_REG_H_
#define _LOG_REG_H_



double f_linear_function(int, double *, double *);
double p_logistic_function(double);
double j_cross_entropy_function(double, int);
double dj_gradient_function(double, int, double);
double *current_observation_weight_training(int, double*, int);
double *model_fitting(int, int, double**, int*);

double *model_fittingV2(HashTable *, Vector *);
double *current_observation_weight_trainingV2(Vector *, Vector *, double *, int );
double f_linear_functionV2(int , double *, double *);
Vector *findTF_IDFvector(HashTable *, char *);
double *calcTF_IDFsum(Vector *, Vector *);


#endif
