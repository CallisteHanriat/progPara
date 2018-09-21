#include <mpi.h>
#include <stdio.h>
int main (int argc, char* argv [])
{
 int nombre, numero;
 /* Initialisation de MPI */
 MPI_Init (&argc, &argv);
 /* Nombre de processus */
 MPI_Comm_size (MPI_COMM_WORLD, &nombre);

 /* Numero du processus appelant */
 MPI_Comm_rank (MPI_COMM_WORLD, &numero);
 printf ("Je suis le processus n° %d", numero);
 printf (" sur un total de %d processus.\n", nombre);
 /* Arrêt de MPI */
 MPI_Finalize();
}