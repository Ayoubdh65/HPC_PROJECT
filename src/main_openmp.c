#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "robot.h"
#include "grille.h"
#include "serveur.h"
#include "rle.h"
#include "utils.h"

// -------------------- helpers --------------------
static int all_reached(Robot robots[], int nb) {
    for (int i = 0; i < nb; ++i) {
        if (robots[i].x != robots[i].tx || robots[i].y != robots[i].ty)
            return 0;
    }
    return 1;
}

char* load_image(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(sz + 1);
    if (!buf) { fclose(f); return NULL; }

    size_t r = fread(buf, 1, sz, f); (void)r;
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

void read_config(const char *path, int *N, int *NB_ROBOTS, int *NB_ITERATIONS) {
    FILE *f = fopen(path, "r");
    if (!f) {
        *N = 20;
        *NB_ROBOTS = 5;
        *NB_ITERATIONS = 50;
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "N=%d", N);
        sscanf(line, "NB_ROBOTS=%d", NB_ROBOTS);
        sscanf(line, "NB_ITERATIONS=%d", NB_ITERATIONS);
    }
    fclose(f);
}

// -------------------- main --------------------
int main(int argc, char **argv) {

    const char *config = (argc > 1) ? argv[1] : "data/config.txt";

    int N, NB_ROBOTS, NB_ITERATIONS;
    read_config(config, &N, &NB_ROBOTS, &NB_ITERATIONS);

    int nthreads = 4;
    if (argc > 2) nthreads = atoi(argv[2]);

    printf("OpenMP Simulation\n");
    printf("Grid = %d x %d | Robots = %d | Iter = %d | Threads = %d\n\n",
           N, N, NB_ROBOTS, NB_ITERATIONS, nthreads);

    // -------- init random generator (NEW) --------
    init_random();

    // -------- grid & robots --------
    Grille g;
    grille_init(&g, N);

    Robot *robots = malloc(sizeof(Robot) * NB_ROBOTS);
    if (!robots) {
        fprintf(stderr, "malloc robots failed\n");
        return 1;
    }

    // -------- random initialization --------
    for (int i = 0; i < NB_ROBOTS; ++i) {
        int sx, sy, tx, ty;

        random_position(N, &sx, &sy);
        random_position(N, &tx, &ty);

        init_robot(&robots[i], i + 1, sx, sy, tx, ty);
        grille_set(&g, sx, sy, robots[i].id);
    }

    // expose for grille_print (targets shown as x)
    robots_global = robots;
    nb_robots_global = NB_ROBOTS;

    Serveur s;
    serveur_init(&s, NB_ROBOTS);

    double t0 = get_time();

    // -------- simulation loop --------
    for (int it = 0; it < NB_ITERATIONS; ++it) {

        printf("\nIteration %d\n", it);

        int (*proposals)[2] = malloc(NB_ROBOTS * sizeof(*proposals));
        if (!proposals) {
            fprintf(stderr, "malloc proposals failed\n");
            break;
        }

        omp_set_num_threads(nthreads);

        #pragma omp parallel for
        for (int i = 0; i < NB_ROBOTS; ++i) {
            int nx, ny;
            compute_next_move(&robots[i], &nx, &ny, N);
            proposals[i][0] = nx;
            proposals[i][1] = ny;
        }

        // central resolution
        serveur_resolve_moves(&g, robots, proposals, NB_ROBOTS);

        grille_print(&g);
        free(proposals);

        if (all_reached(robots, NB_ROBOTS)) {
            printf("All robots have reached their targets at iteration %d — stopping early.\n", it);
            break;
        }
    }

    double t1 = get_time();
    printf("\nTotal OpenMP simulation time: %f s\n", t1 - t0);

    // -------- RLE demo --------
    char *img = load_image("data/image_test.txt");
    if (img) {
        char *c = rle_compress_seq(img);
        char *d = rle_decompress_seq(c);

        printf("\nOriginal image:\n%s\n", img);
        printf("Compressed (RLE):\n%s\n", c);
        printf("Decompressed:\n%s\n", d);

        free(img);
        free(c);
        free(d);
    }

    grille_free(&g);
    free(robots);

    return 0;
}

