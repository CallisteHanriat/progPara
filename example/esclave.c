#include <mpi.h>
#include <stdio.h>
#include "constants.h"
#include "math.h"
#include "stdlib.h"

double averageCalculation(int* tab);
void displayArray(int* array, int rank);
int* voisinDetection(int* grille, int rank, int posX, int posY);

int main( int argc, char *argv[] )
{
  int posX, posY;
  int temperature, myrank;
  int tableauTemperatures[NB_ESCLAVE];
  int longueurDePlaque;
  int grille[LONGUEUR_PLAQUE * HAUTEUR_PLAQUE];
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
    posX = (myrank-1)/LONGUEUR_PLAQUE;
    posY = (myrank-1)%LONGUEUR_PLAQUE;    
   
    for (int i = 0; i<HAUTEUR_PLAQUE*LONGUEUR_PLAQUE; i++) {
      grille[i] = i;
    }

    displayArray(grille, myrank);

    int* listDesVoisins = voisinDetection(grille, myrank, posX, posY);
    printf("list des voisins pour l'esclave %d : ", myrank);
    for (int i = 0; i<8; i++ ) {
      if (listDesVoisins[i] != -1) {
        printf("%d\t", listDesVoisins[i]);
      }      
    }
    printf("\n");

    MPI_Recv(&temperature, 1, MPI_INT, 0, 0, parent, &etat);
    printf ("Fils %d : Esclave : Reception de la temperature de depart : %d !\n", myrank, temperature);
    MPI_Recv(&longueurDePlaque, 1, MPI_INT, 0, 0, parent, &etat);
    printf ("Fils %d : Esclave : Reception de la longueur de la plaque de la part du maître %d!\n", myrank, longueurDePlaque);    
    for (int i = 0; i<NB_TESTS; i++) {
      //Détermination position processeur
      MPI_Recv(&temperature, 1, MPI_INT, 0, 0, parent, &etat);
      printf ("Fils %d : Esclave : Reception de la temperature de depart : %d !\n", myrank, temperature);
      MPI_Recv(&longueurDePlaque, 1, MPI_INT, 0, 0, parent, &etat);
      printf ("Fils %d : Esclave : Reception de la longueur de la plaque de la part du maître %d!\n", myrank, longueurDePlaque);
      for (int j = 0; j<8; j++) {
        if (listDesVoisins[j] != -1 && j != myrank) {
          // MPI_Recv(&temperature, 1, MPI_INT, j, 0, MPI_COMM_WORLD, &etat);
          // printf ("Fils %d : Esclave : Reception de la temperature de depart : %d !\n", myrank, temperature);
          MPI_Send(&temperature, 1, MPI_INT, j, 0, MPI_COMM_WORLD); // Envoie de la temperature aux voisins
          tableauTemperatures[j - myrank] = temperature;
        }
      }
      temperature = averageCalculation(tableauTemperatures);
      printf("Esclave %d: Mise à jour de la temperature %d\n", myrank, temperature);      
      printf("Esclave %d: envoi de la temperature au coordinateur\n", myrank);
    }    
  }

  printf("Esclave : Finalisation esclave %d\n", myrank);
  MPI_Finalize();
  return 0;
}

double averageCalculation(int* tab) {
  int size_t = sizeof(tab)/sizeof(tab[0]);
  int sum = 0;
  for (int i = 0; i<size_t; i++) {
    sum = sum + tab[i];
  }
  return sum/size_t;
}

void displayArray(int* array, int rank) {
  printf("Tableau courant pour le rank %d \n", rank);
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) {
      printf("%d\t", *array);
      *array++;
    }
    printf("\n");
  }
}

int* voisinDetection(int* grille, int rank, int posX, int posY) {
  int* listDesVoisins = malloc(sizeof(int)*8);
  int positionAbsolue = rank+1;

  for (int i = 0; i<8; i++) {
    listDesVoisins[i] = -1;
  }

  if (positionAbsolue <= LONGUEUR_PLAQUE) { // On regarde pas au dessus.
    printf("rank %d On regarde pas au dessus\n", rank);
    if (positionAbsolue%LONGUEUR_PLAQUE == 0) { // On regarde pas à droite
      printf("rank %d On regarde pas à droite\n", rank);
      listDesVoisins[0] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[2] = grille[rank -1];
      return listDesVoisins;
    } else if(positionAbsolue%LONGUEUR_PLAQUE == 1) { // On regarde pas à gauche
      printf("rank %d On regarde pas à gauche\n", rank);
      listDesVoisins[0] = grille[rank + 1];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE+1];
      listDesVoisins[2] = grille[rank + LONGUEUR_PLAQUE];
      return listDesVoisins;
    } else { // On peut voir à droite et à gauche
      printf("rank %d On regarde des deux cotes\n", rank);
      listDesVoisins[0] = grille[rank + 1];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE+1];
      listDesVoisins[2] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[3] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[4] = grille[rank - 1];
      return listDesVoisins;
    }
  }
  if (positionAbsolue > NB_ESCLAVE - LONGUEUR_PLAQUE) { // On regarde pas en dessous
    printf("rank %d On regarde pas au dessous\n", rank);
    printf("rank %d rest de division : rank/longueur (%d / %d)= %d\n", rank,rank,LONGUEUR_PLAQUE, positionAbsolue % LONGUEUR_PLAQUE);
    if (positionAbsolue%LONGUEUR_PLAQUE == 0) { // On regarde pas à droite
      printf("rank %d On regarde pas à droite", rank);
      listDesVoisins[0] = grille[rank-LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank-1];
      listDesVoisins[2] = grille[rank-LONGUEUR_PLAQUE - 1];
      return listDesVoisins;
    } else if (positionAbsolue%LONGUEUR_PLAQUE == 1) { // On regarde pas à gauche
      printf("rank %d On regarde pas à gauche\n", rank);
      listDesVoisins[0] = grille[rank-LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank-LONGUEUR_PLAQUE+1];
      listDesVoisins[2] = grille[rank+1];
      return listDesVoisins;
    } else { // On peut voir à droite et à gauche
      printf("rank %d On regarde des deux cotes\n", rank);
      listDesVoisins[0] = grille[rank-LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank-LONGUEUR_PLAQUE+1];
      listDesVoisins[2] = grille[rank+1];
      listDesVoisins[3] = grille[rank-1];
      listDesVoisins[4] = grille[rank-LONGUEUR_PLAQUE-1];
      return listDesVoisins;
    }
  }
  if (positionAbsolue>=LONGUEUR_PLAQUE && positionAbsolue <= NB_ESCLAVE - LONGUEUR_PLAQUE) { // Si on est ni tout en haut ni tout en bas
    printf("position absolue pour rank %d : %d\n", rank, positionAbsolue);
    if (positionAbsolue%LONGUEUR_PLAQUE == 0) { // On regarde pas à droite
      printf("rank %d avec visuel sur gauche\n", rank);
      listDesVoisins[0] = grille[rank - LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[2] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[3] = grille[rank - 1];
      listDesVoisins[4] = grille[rank - LONGUEUR_PLAQUE -1];
      return listDesVoisins;
    } else if (positionAbsolue%LONGUEUR_PLAQUE == 1) { // On regarde pas à gauche
      printf("rank %d avec visuel sur droite\n", rank);
      listDesVoisins[0] = grille[rank -LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank - LONGUEUR_PLAQUE + 1];
      listDesVoisins[2] = grille[rank +1];
      listDesVoisins[3] = grille[rank + LONGUEUR_PLAQUE + 1];
      listDesVoisins[4] = grille[rank + LONGUEUR_PLAQUE];
      return listDesVoisins;
    } else {
      printf("rank %d avec visuel sur tout\n", rank);
      listDesVoisins[0] = grille[rank - LONGUEUR_PLAQUE];
      listDesVoisins[1] = grille[rank - LONGUEUR_PLAQUE + 1];
      listDesVoisins[2] = grille[rank + 1];
      listDesVoisins[3] = grille[rank + LONGUEUR_PLAQUE + 1];
      listDesVoisins[4] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[5] = grille[rank + LONGUEUR_PLAQUE - 1];
      listDesVoisins[6] = grille[rank - 1];
      listDesVoisins[7] = grille[rank -LONGUEUR_PLAQUE -1];
      return listDesVoisins;
    }
  }
}