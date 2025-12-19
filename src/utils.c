#include "utils.h"
#include <stdlib.h>
#include <time.h>

#ifdef USE_MPI
#include <mpi.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

/* Initialize random generator */
void init_random() {
    srand(time(NULL));
}

/* Random integer in [min, max] */
int rand_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

/* Generate random (x,y) inside grid */
void random_position(int N, int *x, int *y) {
    *x = rand_int(0, N - 1);
    *y = rand_int(0, N - 1);
}

/* Time measurement */
double get_time() {
#ifdef USE_MPI
    return MPI_Wtime();
#elif defined(_OPENMP)
    return omp_get_wtime();
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

