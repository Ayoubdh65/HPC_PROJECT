# Mini Projet HPC — Simulation de coordination de robots & RLE (C, OpenMP, MPI)

## Résumé
Ce projet simule des robots se déplaçant sur une grille et implémente la compression/décompression RLE d'images. Trois versions :
- Séquentielle
- Parallèle OpenMP
- Parallèle MPI (architect. distribuée; rank 0 = serveur/coordinateur)

Langage : C  
Technos : OpenMP, MPI

---

## Prérequis (WSL Ubuntu)
Ouvrez un terminal WSL (Ubuntu) puis installez :

```bash
sudo apt update
sudo apt install -y build-essential gcc libopenmpi-dev openmpi-bin make
"# HPC_PROJECT" 
