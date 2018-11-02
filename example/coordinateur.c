#include <mpi.h>
#include <stdio.h>
#include "constants.h"

void displayArray(double* array);

int main( int argc, char *argv[] )
{
  int myrank, i, j, longueurDePlaque;
  double temperature;
  char return_value=1;
  MPI_Comm parent;
  MPI_Status etat;
  MPI_Init (&argc, &argv);
  MPI_Comm_get_parent (&parent);
  MPI_Comm_rank (MPI_COMM_WORLD,&myrank);  

  double tableauValeurs[NB_ESCLAVE];

  MPI_Info infos[1] = { MPI_INFO_NULL };

  if (parent == MPI_COMM_NULL)
  {
    printf ("Fils %d : Coordinateur : Pas de pere !\n", myrank);
  }
  else {
    MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
    printf ("Fils %d : Coordinateur : Reception de la temperature %2f!\n", myrank, temperature);
    MPI_Recv(&longueurDePlaque, 1, MPI_INT, 0, 0, parent, &etat);
    printf ("Fils %d : Coordinateur : Reception de la longueur de la plaque de la part du maître %d!\n", myrank, longueurDePlaque);
    for (i = 0; i<NB_TESTS; i++) {
      for (j = 1; j<NB_ESCLAVE+1; j++) {
        printf("Coordinateur iteration %d : envoi de la temperature au fils %d\n", i, j);
        MPI_Send(&temperature, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD); // Envoi de la température ambiance aux differents esclaves.
        // MPI_Recv(&tableauValeurs[j-1], 1, MPI_INT, j, 0, MPI_COMM_WORLD, &etat);
      }

      for (j = 1; j<NB_ESCLAVE+1; j++) {
        printf("Coordinateur iteration %d : attente de reception de temperature esclave %d\n", i, j);
        MPI_Recv(&tableauValeurs[j-1], 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD, &etat);
        // MPI_Recv(&tableauValeurs[j-1], 1, MPI_INT, j, 0, MPI_COMM_WORLD, &etat);
        printf("Coordinateur : Reception de la temperature mise à jour d'un fils %d : %2f\n", j, tableauValeurs[j-1]);
      }    
      displayArray(tableauValeurs);
    }
    
    MPI_Send(&return_value, 1, MPI_INT, 0, 0, parent); //Envoi du char au père maitre
  }
  MPI_Finalize();
  return 0;
}

void displayArray(double* array) {
  printf("Tableau courant \n");
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) {
      printf("%2f\t", *array);
      *array++;
    }
    printf("\n");
  }
}