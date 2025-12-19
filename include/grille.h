#ifndef GRILLE_H
#define GRILLE_H

#define EMPTY -1
typedef struct {
    int N;
    int *cells; // 1D array N*N, value = robot id or EMPTY
} Grille;

void grille_init(Grille *g, int N);
void grille_free(Grille *g);
int grille_get(Grille *g, int x, int y);
void grille_set(Grille *g, int x, int y, int val);
int is_free(Grille *g, int x, int y);
void grille_print(const Grille *g);

#endif
