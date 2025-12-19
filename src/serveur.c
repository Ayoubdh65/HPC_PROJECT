#include <stdlib.h>
#include <stdio.h>
#include "serveur.h"
#include "grille.h"
#include "robot.h"

// global variables (for grille_print)
int nb_robots_global = 0;
Robot *robots_global = NULL;

void serveur_init(Serveur *s, int nb_robots)
{
    s->nb_robots = nb_robots;
}

void serveur_resolve_moves(Grille *g, Robot robots[], int proposals[][2], int nb_robots)
{
    int N = g->N;

    int *owner = (int *)malloc(sizeof(int) * N * N);
    if (!owner) {
        fprintf(stderr, "serveur_resolve_moves: malloc failed\n");
        exit(1);
    }

    for (int i = 0; i < N * N; i++)
        owner[i] = -1;

    // ----------------------------
    // FIRST PASS — assign owners by priority (lower ID wins)
    // ----------------------------
    for (int i = 0; i < nb_robots; i++) {

        int rx = proposals[i][0];
        int ry = proposals[i][1];

        // Invalid ? stay in place
        if (rx < 0 || ry < 0 || rx >= N || ry >= N) {
            proposals[i][0] = robots[i].x;
            proposals[i][1] = robots[i].y;
            continue;
        }

        int idx = ry * N + rx;

        int is_target =
            (robots[i].tx == rx && robots[i].ty == ry);

        if (owner[idx] == -1 &&
            (g->cells[idx] == EMPTY ||
             g->cells[idx] == robots[i].id ||
             is_target)) 
        {
            owner[idx] = robots[i].id;
        }
        else {
            // Stay in place
            proposals[i][0] = robots[i].x;
            proposals[i][1] = robots[i].y;
        }
    }

    // ----------------------------
    // SECOND PASS — clear old grid positions
    // ----------------------------
    for (int i = 0; i < nb_robots; i++) {
        int ox = robots[i].x;
        int oy = robots[i].y;

        if (ox >= 0 && oy >= 0 && ox < N && oy < N) {
            int idx = oy * N + ox;
            if (g->cells[idx] == robots[i].id)
                g->cells[idx] = EMPTY;
        }
    }

    // ----------------------------
    // THIRD PASS — apply new robot positions
    // ----------------------------
    for (int i = 0; i < nb_robots; i++) {
        int nx = proposals[i][0];
        int ny = proposals[i][1];

        robots[i].x = nx;
        robots[i].y = ny;

        int idx = ny * N + nx;
        g->cells[idx] = robots[i].id;
    }

    free(owner);
}

