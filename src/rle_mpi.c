#include "rle.h"
#include <stdlib.h>

extern char* rle_compress_seq(const char *input);
extern char* rle_decompress_seq(const char *input);

char* rle_compress_mpi(const char *input) {
    return rle_compress_seq(input);
}

char* rle_decompress_mpi(const char *input) {
    return rle_decompress_seq(input);
}
