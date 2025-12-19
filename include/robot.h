#ifndef ROBOT_H
#define ROBOT_H

#define MAX_ID_LEN 32

typedef struct {
    int id;
    int x, y;
    int tx, ty; // target
    int arrived;
} Robot;

void init_robot(Robot *r, int id, int x, int y, int tx, int ty);
void compute_next_move(Robot *r, int *nx, int *ny, int N);

#endif
