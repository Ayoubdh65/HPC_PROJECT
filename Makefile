CC=gcc
MPICC=mpicc
CFLAGS=-O2 -Wall -Iinclude
OPENMP_FLAGS=-fopenmp

SRC=src
INC=include
BIN_SEQ=main_sequentiel
BIN_OMP=main_openmp
BIN_MPI=main_mpi

.PHONY: all seq openmp mpi clean

all: seq openmp mpi

seq:
	$(CC) $(CFLAGS) $(SRC)/main_sequentiel.c $(SRC)/robot.c $(SRC)/grille.c $(SRC)/serveur.c $(SRC)/rle_sequentiel.c $(SRC)/utils.c -o $(BIN_SEQ)

openmp:
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) \
	src/main_openmp.c src/robot.c src/grille.c src/serveur.c \
	src/rle_openmp.c src/rle_sequentiel.c src/utils.c \
	-o main_openmp

mpi:
	$(MPICC) $(CFLAGS) \
	src/main_mpi.c src/robot.c src/grille.c src/serveur.c \
	src/rle_mpi.c src/rle_sequentiel.c src/utils.c \
	-o main_mpi

clean:
	rm -f $(BIN_SEQ) $(BIN_OMP) $(BIN_MPI) *.o
