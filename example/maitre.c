#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "conf.h"
#include <stdlib.h>

FILE* getFile() {
  char* fileName = "1.conf";
  FILE* f = fopen(fileName, "r");
  return f;
}

int main( int argc, char *argv[]) {
  int i, compteur, nombre;
  // int longueur_plaque = LONGUEUR_PLAQUE, hauteur_plaque=HAUTEUR_PLAQUE;
  FILE* f = getFile();
  int* taille;
  taille = read_size(f);
  int longueur_plaque = taille[0], hauteur_plaque = taille[1];
  printf("maitre : taille[0] : %d, taille[1] : %d\n", taille[0], taille[1]);
  double temperature_depart = 30.0;
  double temperature_chaude = TEMPERATURE_CHAUDE;
  double temperature = read_double(f);
  printf("maitre : lecture de temperature %2f\n", temperature);
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

  double temperature_ambiante = read_double(f);
  printf("Pere : Envoi de temperature ambiance %2f vers coordinateur\n", temperature_ambiante); 

  double seuil = read_double(f);
  fclose(f);
  printf("maitre : Capture du seuil %2f\n", seuil);

  MPI_Send(&seuil, 1, MPI_DOUBLE, 0, 0, intercomm);
  printf("maitre : Envoi du seuil au coordinateur\n");
  
  for (i=1; i<NB_ESCLAVE+1; i++) {
    if (i == 6) {
      MPI_Send (&temperature_chaude,1,MPI_DOUBLE,i,0,intercomm);  // envoi de la temperature chaude à la sixième case.
    } else {
      MPI_Send (&temperature,1,MPI_DOUBLE,i,0,intercomm); // envoi la température de départ aux esclaves.
    }
    
    MPI_Send(&longueur_plaque, 1, MPI_INT, i, 0, intercomm); // envoi de la longueur de la plaque aux esclaves
    printf ("Pere : Envoi vers %d.\n", i);
  }
  printf("Pere : Envoi de la temperature ambiante vers le coordinateur\n");
  MPI_Send(&temperature_ambiante, 1, MPI_DOUBLE, 0, 0, intercomm); // envoi de la température ambiante au coordinateur.
  MPI_Send(&longueur_plaque, 1, MPI_INT, 0, 0, intercomm); // envoi de la longueur de la plaque au coordinateur

  MPI_Recv(&return_value, 1, MPI_INT, 0, 0, intercomm, &etat); // bloquage sur reception du char de la part du coordinateur.

  printf ("Pere : Reception du char du coordinateur. Arret du programme.\n");
  MPI_Finalize();
  return 0;
}