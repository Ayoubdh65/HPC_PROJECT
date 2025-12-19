#include <stdlib.h>
#include "serveur.h"
#include "grille.h"
#include "robot.h"

// =============================
// GLOBAL VARIABLES (DEFINED HERE ONLY)
// =============================
int nb_robots_global = 0;
Robot *robots_global = NULL;

// =============================
// SERVER INITIALIZATION
// =============================
void serveur_init(Serveur *s, int nb_robots) {
    s->nb_robots = nb_robots;
}

// =============================
// COLLISION RESOLUTION LOGIC
// =============================
//
// proposals[i][0] = proposed x
// proposals[i][1] = proposed y
//
// robots[i] holds current position and target
//
// Algorithm:
// 1. Proposals sent by robots
// 2. Server checks conflicts:
//      - If two robots want the same cell → lower ID wins
// 3. Update grid with validated moves
//
// =============================
void serveur_resolve_moves(Grille *g, Robot robots[], int proposals[][2], int nb_robots) {

    // Track ownership of each cell
    int N = g->N;
    int *owner = (int *)malloc(sizeof(int) * N * N);

    for (int i = 0; i < N * N; i++)
        owner[i] = -1; // no owner yet

    // First pass: assign winners by priority (robot ID)
    for (int i = 0; i < nb_robots; i++) {
        int nx = proposals[i][0];
        int ny = proposals[i][1];

        // Clamp inside grid (safety)
        if (nx < 0) nx = 0;
        if (ny < 0) ny = 0;
        if (nx >= N) nx = N - 1;
        if (ny >= N) ny = N - 1;

        int idx = ny * N + nx;

        // If cell free → take it
        if (owner[idx] == -1) {
            owner[idx] = robots[i].id;
        }
        else {
            // Conflict detected → keep smallest ID
            if (robots[i].id < owner[idx]) {
                owner[idx] = robots[i].id;
            }
            // else robot stays in place automatically
        }
    }

    // Second pass: apply resolved moves
    for (int i = 0; i < nb_robots; i++) {
        int nx = proposals[i][0];
        int ny = proposals[i][1];

        // Clamp for safety
        if (nx < 0) nx = 0;
        if (ny < 0) ny = 0;
        if (nx >= N) nx = N - 1;
        if (ny >= N) ny = N - 1;

        int idx = ny * N + nx;

        if (owner[idx] == robots[i].id) {
            // move validated
            robots[i].x = nx;
            robots[i].y = ny;
        }
        // else → robot stays in place
    }

    // Rewrite grid
    grille_clear(g);

    for (int i = 0; i < nb_robots; i++) {
        grille_set(g, robots[i].x, robots[i].y, robots[i].id);
    }

    free(owner);
}
