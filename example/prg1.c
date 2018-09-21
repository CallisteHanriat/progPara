#include <mpi.h>
#include <stdio.h>
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
    printf ("Fils %d : Prg1 : Reception de la temperature de depart : %d !\n", myrank, temperature);
    MPI_Send(&return_value, 1, MPI_INT, 0, 0, parent);
    // printf ("Fils %d : Prg1 : Envoi vers le pere !\n", myrank);
  }
  MPI_Finalize();
  return 0;
}