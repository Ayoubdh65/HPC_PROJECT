#include "robot.h"
#include <stdlib.h>

// Initialize robot
void init_robot(Robot *r, int id, int x, int y, int tx, int ty) {
    r->id = id;
    r->x = x;
    r->y = y;
    r->tx = tx;
    r->ty = ty;
}

void compute_next_move(Robot *r, int *nx, int *ny, int N) {

    *nx = r->x;
    *ny = r->y;

    // If robot already arrived, do nothing
    if (r->x == r->tx && r->y == r->ty) 
        return;

    // Move horizontally first
    if (r->x < r->tx) {
        *nx = r->x + 1;
        return;
    }
    if (r->x > r->tx) {
        *nx = r->x - 1;
        return;
    }

    // If x is aligned, move vertically
    if (r->y < r->ty) {
        *ny = r->y + 1;
        return;
    }
    if (r->y > r->ty) {
        *ny = r->y - 1;
        return;
    }
}


