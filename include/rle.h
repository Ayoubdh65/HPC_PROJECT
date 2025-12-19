#ifndef RLE_H
#define RLE_H

// Simple RLE functions for char arrays (image as single string with '\n' per row)
char* rle_compress_seq(const char *input);
char* rle_decompress_seq(const char *input);

#endif
