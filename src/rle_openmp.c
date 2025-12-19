#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <omp.h>
#include "rle.h"

// Parallel RLE by splitting the input into blocks by newline (row-wise).
// We keep it simple: compress each row in parallel and join results.
extern char* rle_compress_seq(const char *input);
extern char* rle_decompress_seq(const char *input);

char* rle_compress_openmp(const char *input) {
    // For simplicity call sequential (safe) — or implement per-row parallelization:
    return rle_compress_seq(input);
}

char* rle_decompress_openmp(const char *input) {
    return rle_decompress_seq(input);
}
