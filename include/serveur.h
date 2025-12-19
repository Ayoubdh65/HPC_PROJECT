#ifndef SERVEUR_H
#define SERVEUR_H

#include "robot.h"
#include "grille.h"

// ============================
// Global variables (DECLARATION only)
// ============================
extern int nb_robots_global;
extern Robot *robots_global;

// ============================
// Server structure
// ============================
typedef struct {
    int nb_robots;
} Serveur;

// ============================
// Functions
// ============================
void serveur_init(Serveur *s, int nb_robots);
void serveur_resolve_moves(Grille *g, Robot robots[], int proposals[][2], int nb_robots);

#endif

