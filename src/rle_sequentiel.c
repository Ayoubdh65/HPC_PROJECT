#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rle.h"

char* rle_compress_seq(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    size_t bufsize = (len * 4) + 10;
    char *out = (char*)malloc(bufsize);
    size_t pos = 0;
    size_t i = 0;
    while (i < len) {
        char c = input[i];
        if (c == '\n') {
            out[pos++] = '\n';
            i++;
            continue;
        }
        size_t j = i+1;
        while (j < len && input[j] == c && input[j] != '\n') j++;
        int count = (int)(j - i);
        // write char then count as decimal then '|'
        int n = sprintf(out+pos, "%c%d|", c, count);
        pos += n;
        i = j;
    }
    out[pos] = '\0';
    return out;
}

char* rle_decompress_seq(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    size_t bufsize = len * 10 + 10;
    char *out = (char*)malloc(bufsize);
    size_t pos = 0;
    size_t i = 0;
    while (i < len) {
        if (input[i] == '\n') {
            out[pos++] = '\n';
            i++;
            continue;
        }
        char c = input[i++];
        int count = 0;
        while (i < len && input[i] >= '0' && input[i] <= '9') {
            count = count*10 + (input[i]-'0');
            i++;
        }
        if (i < len && input[i] == '|') i++;
        for (int k=0;k<count;k++) out[pos++] = c;
    }
    out[pos] = '\0';
    return out;
}
