#include <mpi.h>
#include <stdio.h>
#include "constants.h"
#include "math.h"
#include "stdlib.h"

double averageCalculation(double* tab, double actual_temp);
void displayArray(int* array, int rank);
int* voisinDetection(int* grille, int rank, int posX, int posY);

int main( int argc, char *argv[] )
{
  int posX, posY;
  int myrank;
  double temperature;
  double tableauTemperatures[8];
  int longueurDePlaque;
  double temperature_ambiante;
  int grille[LONGUEUR_PLAQUE * HAUTEUR_PLAQUE];
  char return_value=1;
  MPI_Comm parent;
  MPI_Status etat;
  MPI_Init (&argc, &argv);
  MPI_Comm_get_parent (&parent);
  MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
  MPI_Request req;

  if (parent == MPI_COMM_NULL)
  {
    printf ("Fils %d : Prg1 : Pas de pere !\n", myrank);
  }
  else {
    posX = (myrank-1)/LONGUEUR_PLAQUE;
    posY = (myrank-1)%LONGUEUR_PLAQUE;    
   
    for (int i = 0; i<HAUTEUR_PLAQUE*LONGUEUR_PLAQUE; i++) {
      grille[i] = i+1;
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

    MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
    printf ("Fils %d : Esclave : Reception de la temperature de depart : %2f !\n", myrank, temperature);
    MPI_Recv(&longueurDePlaque, 1, MPI_INT, 0, 0, parent, &etat);
    printf ("Fils %d : Esclave : Reception de la longueur de la plaque de la part du maître %d!\n", myrank, longueurDePlaque);    
    for (int i = 0; i<NB_TESTS; i++) {
      //Détermination position processeur
      MPI_Recv(&temperature_ambiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
      printf ("Fils %d : Esclave iteration %d: Reception de la temperature ambiante : %2f ! ", myrank, i, temperature_ambiante);
      if(temperature_ambiante < -140.0) {
        printf("temperature tres froide : break\n");
        break;
      }
      for (int j = 0; j<8; j++) {
        if (listDesVoisins[j] != 0 && listDesVoisins[j] != myrank) {
          printf("Fils %d : Esclave : Envoie de la temperature au fils %d\n", myrank, listDesVoisins[j]);
          MPI_Isend(&temperature, 1, MPI_DOUBLE, listDesVoisins[j], 0, MPI_COMM_WORLD, &req); // Envoie de la temperature aux voisins
        }
      }
      for(int j = 0; j<8; j++) {
        // if (listDesVoisins[j] != -1){
        //   printf("Fils %d : Esclave : Attente de reception de la temperature", myrank);
        //   MPI_Recv(&temperature, 1, MPI_INT, j, 0, MPI_COMM_WORLD, &etat);
        //   printf("Fils %d : Esclave : Reception de la temperature provenant de %d\n", myrank, listDesVoisins[j]);
        // }
        if (listDesVoisins[j] != 0 && listDesVoisins[j] != myrank) {
          printf("Fils %d : Esclave : Attente de reception du fils %d\n", myrank, listDesVoisins[j]);
          MPI_Recv(&tableauTemperatures[j], 1, MPI_DOUBLE, listDesVoisins[j], 0, MPI_COMM_WORLD, &etat);
          printf("Fils %d : Esclave : Reception de la temperature provenant de %d : %2f\n", myrank, listDesVoisins[j], tableauTemperatures[j]);  
        } else {
          tableauTemperatures[j] = 20.0;
        }        
      }
      temperature = averageCalculation(tableauTemperatures, temperature);
      printf("Esclave %d: Mise à jour de la temperature %2f\n", myrank, temperature);
      printf("Esclave %d: envoi de la temperature au coordinateur\n", myrank);
      MPI_Send(&temperature, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }    
  }

  printf("Esclave : Finalisation esclave %d\n", myrank);
  MPI_Finalize();
  return 0;
}

double averageCalculation(double* tab, double actual_temp) {
  int size_t = sizeof(tab)/sizeof(tab[0]);
  double sum = 0;
  printf("Calcul de la moyene\n");
  for (int i = 0; i<8; i++) {

    printf("%2f = %2f + %2f (tab[%d])\n", sum, sum, tab[i], i);
    sum = sum + tab[i];
  }
  sum = sum + actual_temp;
  printf("Current SUM : %2f\n", sum);
  return sum/9.0;
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

/**
 * Cette fonction doit retourner la liste des voisins (les id des voisins)
 * */
int* voisinDetection(int* grille, int rank, int posX, int posY) {
  int* listDesVoisins = malloc(sizeof(int)*8);
  int numeroDansLaGrille = rank;

  for (int i = 0; i<8; i++) {
    listDesVoisins[i] = 0;
  }

/*   if(numeroDansLaGrille == NB_ESCLAVE) {
    return listDesVoisins;
  }
 */
  if (numeroDansLaGrille <= LONGUEUR_PLAQUE) { // On regarde pas au dessus.
    printf("rank %d On regarde pas au dessus\n", rank);
    if (numeroDansLaGrille%LONGUEUR_PLAQUE == 0) { // On regarde pas à droite
      printf("rank %d On regarde pas à droite\n", rank);
      listDesVoisins[0] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE-2];
      listDesVoisins[2] = grille[rank -2];
      return listDesVoisins;
    } else if(numeroDansLaGrille%LONGUEUR_PLAQUE == 1) { // On regarde pas à gauche
      printf("rank %d On regarde pas à gauche\n", rank);
      listDesVoisins[0] = grille[rank];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[2] = grille[rank + LONGUEUR_PLAQUE-1];
      return listDesVoisins;
    } else { // On peut voir à droite et à gauche
      printf("rank %d On regarde des deux cotes\n", rank);
      listDesVoisins[0] = grille[rank];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[2] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[3] = grille[rank + LONGUEUR_PLAQUE-2];
      listDesVoisins[4] = grille[rank-2];
      return listDesVoisins;
    }
  }
  if (numeroDansLaGrille > NB_ESCLAVE - LONGUEUR_PLAQUE) { // On regarde pas en dessous
    printf("rank %d On regarde pas au dessous\n", rank);
    printf("rank %d rest de division : rank/longueur (%d / %d)= %d\n", rank,rank,LONGUEUR_PLAQUE, numeroDansLaGrille % LONGUEUR_PLAQUE);
    if (numeroDansLaGrille%LONGUEUR_PLAQUE == 0) { // On regarde pas à droite
      printf("rank %d On regarde pas à droite", rank);
      listDesVoisins[0] = grille[rank-LONGUEUR_PLAQUE-1];
      listDesVoisins[1] = grille[rank-2];
      listDesVoisins[2] = grille[rank-LONGUEUR_PLAQUE - 2];
      return listDesVoisins;
    } else if (numeroDansLaGrille%LONGUEUR_PLAQUE == 1) { // On regarde pas à gauche
      printf("rank %d On regarde pas à gauche\n", rank);
      listDesVoisins[0] = grille[rank-LONGUEUR_PLAQUE-1];
      listDesVoisins[1] = grille[rank-LONGUEUR_PLAQUE];
      listDesVoisins[2] = grille[rank];
      return listDesVoisins;
    } else { // On peut voir à droite et à gauche
      printf("rank %d On regarde des deux cotes\n", rank);
      listDesVoisins[0] = grille[rank-LONGUEUR_PLAQUE-1];
      listDesVoisins[1] = grille[rank-LONGUEUR_PLAQUE];      
      listDesVoisins[2] = grille[rank];
      listDesVoisins[3] = grille[rank-2];
      listDesVoisins[4] = grille[rank-LONGUEUR_PLAQUE-2];
      return listDesVoisins;
    }
  }
  if (numeroDansLaGrille>=LONGUEUR_PLAQUE && numeroDansLaGrille <= NB_ESCLAVE - LONGUEUR_PLAQUE) { // Si on est ni tout en haut ni tout en bas
    printf("position absolue pour rank %d : %d\n", rank, numeroDansLaGrille);
    if (numeroDansLaGrille%LONGUEUR_PLAQUE == 0) { // On regarde pas à droite
      printf("rank %d avec visuel sur gauche\n", rank);
      listDesVoisins[0] = grille[rank - LONGUEUR_PLAQUE-1];
      listDesVoisins[1] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[2] = grille[rank + LONGUEUR_PLAQUE-2];
      listDesVoisins[3] = grille[rank-2];
      listDesVoisins[4] = grille[rank - LONGUEUR_PLAQUE-2];
      return listDesVoisins;
    } else if (numeroDansLaGrille%LONGUEUR_PLAQUE == 1) { // On regarde pas à gauche
      printf("rank %d avec visuel sur droite\n", rank);
      listDesVoisins[0] = grille[rank -LONGUEUR_PLAQUE-1];
      listDesVoisins[1] = grille[rank - LONGUEUR_PLAQUE];
      listDesVoisins[2] = grille[rank];
      listDesVoisins[3] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[4] = grille[rank + LONGUEUR_PLAQUE-1];
      return listDesVoisins;
    } else {
      printf("rank %d avec visuel sur tout\n", rank);
      listDesVoisins[0] = grille[rank - LONGUEUR_PLAQUE - 1];
      listDesVoisins[1] = grille[rank - LONGUEUR_PLAQUE];      
      listDesVoisins[2] = grille[rank];
      listDesVoisins[3] = grille[rank + LONGUEUR_PLAQUE];
      listDesVoisins[4] = grille[rank + LONGUEUR_PLAQUE-1];
      listDesVoisins[5] = grille[rank + LONGUEUR_PLAQUE-2];      
      listDesVoisins[6] = grille[rank - 2];
      listDesVoisins[7] = grille[rank -LONGUEUR_PLAQUE -2];
      return listDesVoisins;
    }
  }
}