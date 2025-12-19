#include "grille.h"
#include "serveur.h"
#include <stdio.h>
#include <stdlib.h>

void grille_init(Grille *g, int N) {
    g->N = N;
    g->cells = malloc(sizeof(int) * N * N);
    if (!g->cells) {
        fprintf(stderr, "grille_init: malloc failed\n");
        exit(1);
    }

    for (int i = 0; i < N * N; i++)
        g->cells[i] = EMPTY;   // use EMPTY consistently
}

void grille_clear(Grille *g) {
    for (int i = 0; i < g->N * g->N; i++)
        g->cells[i] = EMPTY;
}

void grille_set(Grille *g, int x, int y, int id) {
    g->cells[y * g->N + x] = id;
}

void grille_free(Grille *g) {
    if (g->cells) free(g->cells);
    g->cells = NULL;
}

void grille_print(const Grille *g) {

    printf("\n");
    for (int y = 0; y < g->N; y++) {
        for (int x = 0; x < g->N; x++) {

            int id = g->cells[y * g->N + x];

            // If robot is here → print robot ID
            if (id != EMPTY) {
                printf("%d ", id);
                continue;
            }

            // If this is a target position → print 'x'
            int printed = 0;
            for (int i = 0; i < nb_robots_global; i++) {
                if (robots_global[i].tx == x && robots_global[i].ty == y) {
                    printf("x ");
                    printed = 1;
                    break;
                }
            }

            if (!printed)
                printf(". ");
        }
        printf("\n");
    }
    printf("\n");
}

