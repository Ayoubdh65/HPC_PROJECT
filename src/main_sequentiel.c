#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "robot.h"
#include "grille.h"
#include "serveur.h"
#include "utils.h"
#include "rle.h"

// -------- AUTO-STOP CHECK --------
static int all_reached(Robot robots[], int nb) {
    for (int i = 0; i < nb; i++) {
        if (robots[i].x != robots[i].tx || robots[i].y != robots[i].ty)
            return 0;
    }
    return 1;
}

// -------- LOAD IMAGE FILE --------
char* load_image(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(sz + 1);
    size_t r = fread(buf, 1, sz, f); (void)r;
    buf[sz] = '\0';

    fclose(f);
    return buf;
}

// -------- READ CONFIG --------
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

// -------- MAIN --------
int main(int argc, char **argv) {

    const char *config = (argc > 1) ? argv[1] : "data/config.txt";

    int N, NB_ROBOTS, NB_ITERATIONS;
    read_config(config, &N, &NB_ROBOTS, &NB_ITERATIONS);

    printf("Sequential Simulation\n");
    printf("Grid %d x %d | Robots %d | Iter %d\n\n",
           N, N, NB_ROBOTS, NB_ITERATIONS);

    // Init random generator (NEW)
    init_random();

    // Init grid
    Grille g;
    grille_init(&g, N);

    // Init robots
    Robot *robots = malloc(sizeof(Robot) * NB_ROBOTS);

    for (int i = 0; i < NB_ROBOTS; i++) {
        int sx, sy, tx, ty;

        random_position(N, &sx, &sy);
        random_position(N, &tx, &ty);

        init_robot(&robots[i], i + 1, sx, sy, tx, ty);
        grille_set(&g, sx, sy, robots[i].id);
    }

    // Expose global robots for printing targets
    robots_global = robots;
    nb_robots_global = NB_ROBOTS;

    Serveur s;
    serveur_init(&s, NB_ROBOTS);

    double t0 = get_time();

    for (int it = 0; it < NB_ITERATIONS; it++) {

        printf("\nIteration %d\n", it);

        int (*proposals)[2] = malloc(NB_ROBOTS * sizeof(*proposals));

        for (int i = 0; i < NB_ROBOTS; i++) {
            int nx, ny;
            compute_next_move(&robots[i], &nx, &ny, N);
            proposals[i][0] = nx;
            proposals[i][1] = ny;
        }

        serveur_resolve_moves(&g, robots, proposals, NB_ROBOTS);
        grille_print(&g);

        free(proposals);

        if (all_reached(robots, NB_ROBOTS)) {
            printf("All robots have reached their targets at iteration %d — stopping early.\n", it);
            break;
        }
    }

    double t1 = get_time();
    printf("\nSequential simulation time = %f s\n", t1 - t0);

    // -------- RLE DEMONSTRATION --------
    char *img = load_image("data/image_test.txt");
    if (img) {
        char *comp = rle_compress_seq(img);
        char *decomp = rle_decompress_seq(comp);

        printf("\nOriginal:\n%s\n", img);
        printf("Compressed:\n%s\n", comp);
        printf("Decompressed:\n%s\n", decomp);

        free(img);
        free(comp);
        free(decomp);
    }

    grille_free(&g);
    free(robots);

    return 0;
}

