#ifndef UTILS_H
#define UTILS_H

/* Time measurement */
double get_time();

/* Random utilities */
void init_random();
int rand_int(int min, int max);

/* Generate random start/target positions */
void random_position(int N, int *x, int *y);

#endif

