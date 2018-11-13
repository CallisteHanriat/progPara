#include <mpi.h>
#include <stdio.h>
#include "tgmath.h"
#include "constants.h"
#include "conf.h"
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <cairo/cairo-svg.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

MPI_Comm parent;
MPI_Status etat;
GtkWidget *toile;
cairo_t *canvas;
double tableauValeurs[NB_ESCLAVE];
int *taille;
void dessiner();

  int myrank, i, j, longueurDePlaque;

void displayArrayFenetre();
double temperature;
double calcul_somme_temperature(double *array);
double seuil = SEUIL;
float somme_temperature, precedente_somme_temperature = 0.0;
double* tmps;
int iteration = 0;
char return_value = 1;

gboolean on_draw_event(GtkWidget *widget, cairo_t *contexte, gpointer user_data)
{
  canvas = contexte;
  dessiner(/*(draw_req*)user_data*/);
  return FALSE;
}

gboolean time_handler(GtkWidget *widget)
{

  gtk_widget_queue_draw(widget);
  somme_temperature = calcul_somme_temperature(tmps);
  printf("Coordinateur : somme_temperature : %2f \n precedente_somme_temperature : %2f \n seuil : %2f", somme_temperature, precedente_somme_temperature, seuil);
    if (fabs(somme_temperature - precedente_somme_temperature) < fabs(seuil))
  {
    printf("Coordinateur : somme_temperature - precedente_somme_temperature < seuil\n");
    double temp = -150.0;
    for (int j = 1; j < NB_ESCLAVE + 1; j++)
    {
      printf("Coordinateur iteration : envoi de la temperature au fils %d\n", j);
      MPI_Send(&temp, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD); // Envoi de la température ambiance aux differents esclaves.
    }
    printf("Coordinateur: Envoi d'un caractere au pere\n");
    MPI_Send(&return_value, 1, MPI_INT, 0, 0, parent); //Envoi du char au père maitre
    gtk_widget_destroy(widget);        
    return FALSE;
  }
  precedente_somme_temperature = somme_temperature;

  return TRUE;
}

void draw_cell(int x, int y, float scale, int xsize, int ysize, float val)
{

  cairo_set_source_rgb(canvas, 0, 0, 0);
  cairo_rectangle(canvas, x * xsize, y * ysize, (x + 1) * xsize, (y + 1) * ysize);
  cairo_stroke_preserve(canvas);

  cairo_set_source_rgb(canvas, 0, 0.0, 1.0 - 0);
  cairo_fill(canvas);
  cairo_stroke(canvas);

  cairo_set_source_rgb(canvas, 0.1, 0.1, 0.1);
  cairo_select_font_face(canvas, "Courrier",
                         CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(canvas, 15);
  cairo_move_to(canvas, (x + 0.25) * xsize, (y + 0.5) * ysize);
  char c[5];
  sprintf(c, "%0.2f", val);
  cairo_show_text(canvas, c);
}

void dessiner()
{
  printf("\n---------------------------------\nIteration de dessin : %d\n", iteration);
  iteration++;
  cairo_set_source_rgb(canvas, 0, 0, 0);
  cairo_rectangle(canvas, 40, 30, 720, 540);
  cairo_stroke_preserve(canvas);
  cairo_set_source_rgb(canvas, 0, 1, 0);
  cairo_fill(canvas);
  cairo_stroke(canvas);
  for (int j = 1; j < NB_ESCLAVE + 1; j++)
  {
    printf("Coordinateur : envoi de la temperature au fils %d\n", j);
    MPI_Send(&temperature, 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD); // Envoi de la température ambiance aux differents esclaves.
    // MPI_Recv(&tableauValeurs[j-1], 1, MPI_INT, j, 0, MPI_COMM_WORLD, &etat);
  }

  for (int j = 1; j < NB_ESCLAVE + 1; j++)
  {
    printf("Coordinateur iteration : attente de reception de temperature esclave %d\n", j);
    MPI_Recv(&tableauValeurs[j - 1], 1, MPI_DOUBLE, j, 0, MPI_COMM_WORLD, &etat);
    // MPI_Recv(&tableauValeurs[j-1], 1, MPI_INT, j, 0, MPI_COMM_WORLD, &etat);
    printf("Coordinateur : Reception de la temperature mise à jour d'un fils %d : %2f\n", j, tableauValeurs[j - 1]);
  }
  tmps = tableauValeurs;
  printf("Coordinateur : taille[0] = %d ; taille[1] = %d\n", taille[0], taille[1]);
  int xsize = 800 / taille[0], ysize = 600 / taille[1];
  /*if (!max_calculated){
        calc_max();
    }*/

  for (int x = 0; x < taille[0]; x++)
  {
    for (int y = 0; y < taille[1]; y++)
    {
      //float val = (tmps[y*taille[1]+x]-min)/(max-min);
      // draw_cell(x, y, powf(val, 0.2), xsize, ysize, tmps[y*taille[1]+x]);
      printf("tmp[y*taille[1]+x] = %2f\n", tmps[y * taille[1] + x]);
      draw_cell(x, y, 0, xsize, ysize, tmps[y * taille[1] + x]);
    }
  }
}

void displayArray(double *array);

void createWindow(int argc, char *argv[]);
int main(int argc, char *argv[])
{
  
  MPI_Init(&argc, &argv);
  MPI_Comm_get_parent(&parent);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  taille = malloc(sizeof(int) * 2);
  taille[0] = 4;
  taille[1] = 3;

  MPI_Info infos[1] = {MPI_INFO_NULL};

  if (parent == MPI_COMM_NULL)
  {
    printf("Fils %d : Coordinateur : Pas de pere !\n", myrank);
  }
  else
  {

    createWindow(argc, argv);
    MPI_Recv(&seuil, 1, MPI_DOUBLE, 0, 0, parent, &etat);
    printf("Fils %2f: Coordinateur : Recepetion du seuil de la part du maitre\n", seuil);
    MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
    printf("Fils %d : Coordinateur : Reception de la temperature %2f!\n", myrank, temperature);
    MPI_Recv(&longueurDePlaque, 1, MPI_INT, 0, 0, parent, &etat);
    printf("Fils %d : Coordinateur : Reception de la longueur de la plaque de la part du maître %d!\n", myrank, longueurDePlaque);

    // displayArrayFenetre(tableauValeurs);

    



    MPI_Finalize();
  }
  return 0;
}

void displayArrayFenetre()
{
  // tmps = array;
  dessiner();
}

void createWindow(int argc, char *argv[])
{
  GtkWidget *fenetre;
  gtk_init(&argc, &argv);
  fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_position(GTK_WINDOW(fenetre), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(fenetre), 800, 600);
  gtk_window_set_title(GTK_WINDOW(fenetre), "Pacman GTK Cairo");

  g_signal_connect(G_OBJECT(fenetre), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  toile = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(fenetre), toile);
  g_signal_connect(G_OBJECT(toile), "draw", G_CALLBACK(on_draw_event), NULL);
  gtk_widget_show_all(fenetre);
  g_timeout_add(750, (GSourceFunc)time_handler, (gpointer)fenetre);
  gtk_main();
}

void displayArray(double *array)
{
  printf("Tableau courant \n");
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      printf("%2f\t", *array);
      *array++;
    }
    printf("\n");
  }
}

double calcul_somme_temperature(double *array)
{
  double return_value = 0;
  for (int i = 0; i < NB_ESCLAVE; i++)
  {
    return_value += *array;
    *array++;
  }

  return return_value;
}