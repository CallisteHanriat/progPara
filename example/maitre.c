#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

int main( int argc, char *argv[] )
{
  int i, compteur, nombre;
  MPI_Status etat;
  char return_value=1;
  printf("Pere : J'execute avec un nombre d'esclaves  = %s\n", argv[1]);
  int nombre_prog1 = atoi(argv[1]);

  char *cmds[3] = {
    "coordinateur", 
    "esclave"
  };

  int np[2] = {
    1, // On lance 1 instance du programme coordinateur
    NB_ESCLAVE // On lance NB_ESCLAVE esclaves
  };
  // Pas d'info supplémentaire pour contrôler le lancement
  // des programmes 1 et 2
  MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
  int errcodes[NB_ESCLAVE]; // Les codes de retours des 5 processus
  MPI_Comm intercomm; // L'espace de communication père - fils  
  /* Initialisation de MPI */
  MPI_Init (&argc, &argv);
  /* Nombre de processus */
  MPI_Comm_size (MPI_COMM_WORLD, &nombre);

  MPI_Comm_spawn_multiple (
        2,
        cmds,
        MPI_ARGVS_NULL,
        np,
        infos,
        0,
        MPI_COMM_WORLD,
        &intercomm,
        errcodes
    );
  printf ("Pere : J'ai lance toutes les instances.\n");
  // Le père communique de façon synchrone avec chacun de
  // ses fils en utilisant l'espace de communication intercomm

  int temperature_ambiante = 20;
  printf("Pere : Envoi de temperature ambiance vers coordinateur\n"); 
  
  for (i=1; i<NB_ESCLAVE+1; i++) {
    MPI_Send (&temperature_ambiante,1,MPI_INT,i,0,intercomm); // envoi la température de départ au fils.
    printf ("Pere : Envoi vers %d.\n", i);
  }
  MPI_Send(&temperature_ambiante, 1, MPI_INT, 0, 0, intercomm); // envoi de la température ambiante au coordinateur.

  MPI_Recv(&return_value, 1, MPI_INT, 0, 0, intercomm, &etat); // bloquage sur reception du char de la part du coordinateur.

  printf ("Pere : Reception du char du coordinateur. Arret du programme.\n");
  MPI_Finalize();
  return 0;
}