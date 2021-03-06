#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
  int i, compteur, nombre;
  MPI_Status etat;
  char return_value=1;
  printf("Pere : J'execute avec argc  = %s\n", argv[1]);
  int nombre_prog1 = atoi(argv[1]);

  char *cmds[3] = {
    "coordinateur",
    "prg1"
  };

  int np[2] = {
    1, // On lance 1 instance du programme coordinateur
    nombre_prog1 // On lance nombre_prog1 instances du programme 1
  };
  // Pas d'info supplémentaire pour contrôler le lancement
  // des programmes 1 et 2
  MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
  int errcodes[5]; // Les codes de retours des 5 processus
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
  int temperature_depart = 18;
  MPI_Send(&temperature_ambiante, 1, MPI_INT, 0, 0, intercomm); // envoi de la température ambiante au coordinateur.
  for (i=1; i<4; i++)
  {
    MPI_Send (&temperature_depart,1,MPI_INT,i,0,intercomm);
    printf ("Pere : Envoi vers %d.\n", i);
    // MPI_Recv(&compteur, 1, MPI_INT,i, 0, intercomm, &etat);
    // printf ("Pere : Reception de %d.\n", i);
  }

  for (i = 0; i<4; i++) {
    MPI_Recv(&return_value, 1, MPI_INT,i, 0, intercomm, &etat);
    printf("Pere : reception du char provenant de %d\n", i);
  }
  printf ("Pere : Tous les chars ont été recus.\n");
  MPI_Finalize();
  return 0;
}