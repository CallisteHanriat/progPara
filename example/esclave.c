#include <mpi.h>
#include <stdio.h>
#include "constants.h"

int main( int argc, char *argv[] )
{
  int temperature, myrank;
  char return_value=1;
  MPI_Comm parent;
  MPI_Status etat;
  MPI_Init (&argc, &argv);
  MPI_Comm_get_parent (&parent);
  MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

  if (parent == MPI_COMM_NULL)
  {
    printf ("Fils %d : Prg1 : Pas de pere !\n", myrank);
  }
  else {
    MPI_Recv(&temperature, 1, MPI_INT, 0, 0, parent, &etat);
    printf ("Fils %d : Esclave : Reception de la temperature de depart : %d !\n", myrank, temperature);
    for (int j = 0; j<NB_TESTS; j++) {
      MPI_Recv(&temperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &etat);
      printf ("Fils %d : Esclave : Reception de la temperature de depart : %d !\n", myrank, temperature);
      MPI_Send(&temperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // Renvoi de la température au coordinateur      
      printf("Esclave %d: envoi de la temperature au coordinateur\n", myrank);
    }
  }

  printf("Esclave : Finalisation esclave %d\n", myrank);
  MPI_Finalize();
  return 0;
}