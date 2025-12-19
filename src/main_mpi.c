#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

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
        *NB_ROBOTS = 3;
        *NB_ITERATIONS = 30;
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

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *config = (argc > 1) ? argv[1] : "data/config.txt";

    int N, NB_ROBOTS_cfg, NB_ITERATIONS;

    if (rank == 0)
        read_config(config, &N, &NB_ROBOTS_cfg, &NB_ITERATIONS);

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&NB_ROBOTS_cfg, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&NB_ITERATIONS, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int actual_robots = NB_ROBOTS_cfg;
    if (actual_robots > size - 1)
        actual_robots = size - 1;

    if (actual_robots < 1) {
        if (rank == 0)
            fprintf(stderr, "Need at least one robot process.\n");
        MPI_Finalize();
        return 1;
    }

    // -------------------- SERVER --------------------
    if (rank == 0) {

        printf("MPI Simulation (server)\n");
        printf("Grid = %d x %d | Robots = %d | Iter = %d | MPI procs = %d\n\n",
               N, N, actual_robots, NB_ITERATIONS, size);

        // init random ONLY on server
        init_random();

        Grille g;
        grille_init(&g, N);

        Robot *robots = malloc(sizeof(Robot) * actual_robots);
        if (!robots) MPI_Abort(MPI_COMM_WORLD, 1);

        // -------- RANDOM INITIALIZATION --------
        for (int i = 0; i < actual_robots; ++i) {
            int sx, sy, tx, ty;

            random_position(N, &sx, &sy);
            random_position(N, &tx, &ty);

            init_robot(&robots[i], i + 1, sx, sy, tx, ty);
            grille_set(&g, sx, sy, robots[i].id);

            // send init data to robot process
            int init_data[4] = { sx, sy, tx, ty };
            MPI_Send(init_data, 4, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }

        // expose targets for printing
        robots_global = robots;
        nb_robots_global = actual_robots;

        Serveur s;
        serveur_init(&s, actual_robots);

        double t0 = get_time();

        for (int it = 0; it < NB_ITERATIONS; ++it) {

            printf("\nIteration %d\n", it);

            // command robots to compute proposals
            int command = 1;
            for (int r = 1; r <= actual_robots; ++r)
                MPI_Send(&command, 1, MPI_INT, r, 0, MPI_COMM_WORLD);

            int (*proposals)[2] = malloc(actual_robots * sizeof(*proposals));
            if (!proposals) MPI_Abort(MPI_COMM_WORLD, 1);

            // collect proposals
            for (int r = 1; r <= actual_robots; ++r)
                MPI_Recv(proposals[r - 1], 2, MPI_INT, r, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // resolve collisions
            serveur_resolve_moves(&g, robots, proposals, actual_robots);

            // send validated positions
            for (int r = 1; r <= actual_robots; ++r) {
                int validated[2] = { robots[r - 1].x, robots[r - 1].y };
                MPI_Send(validated, 2, MPI_INT, r, 0, MPI_COMM_WORLD);
            }

            grille_print(&g);
            free(proposals);

            if (all_reached(robots, actual_robots)) {
                printf("All robots reached targets at iteration %d — stopping early.\n", it);
                break;
            }
        }

        // send stop signal
        int stop = 0;
        for (int r = 1; r <= actual_robots; ++r)
            MPI_Send(&stop, 1, MPI_INT, r, 0, MPI_COMM_WORLD);

        double t1 = get_time();
        printf("\nMPI server simulation time = %f s\n", t1 - t0);

        grille_free(&g);
        free(robots);
    }

    // -------------------- ROBOT PROCESS --------------------
    else {
        Robot r;

        // receive initial data from server
        int init_data[4];
        MPI_Recv(init_data, 4, MPI_INT, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        init_robot(&r, rank, init_data[0], init_data[1],
                   init_data[2], init_data[3]);

        while (1) {
            int command;
            MPI_Recv(&command, 1, MPI_INT, 0, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (command == 0)
                break;

            int nx, ny;
            compute_next_move(&r, &nx, &ny, N);

            int proposal[2] = { nx, ny };
            MPI_Send(proposal, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);

            int validated[2];
            MPI_Recv(validated, 2, MPI_INT, 0, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            r.x = validated[0];
            r.y = validated[1];
        }
    }

    MPI_Finalize();
    return 0;
}

