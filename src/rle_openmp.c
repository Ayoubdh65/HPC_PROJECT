#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <omp.h>
#include "rle.h"


extern char* rle_compress_seq(const char *input);
extern char* rle_decompress_seq(const char *input);

char* rle_compress_openmp(const char *input) {
    
    return rle_compress_seq(input);
}

char* rle_decompress_openmp(const char *input) {
    return rle_decompress_seq(input);
}
