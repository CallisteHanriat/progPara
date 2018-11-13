#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <cairo/cairo-svg.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <stdio.h>

cairo_t* canvas;
void dessiner()
{
    cairo_set_source_rgb(canvas, 0, 0, 0);
    cairo_rectangle(canvas, 40, 30, 720, 540);
    cairo_stroke_preserve(canvas);
    cairo_set_source_rgb (canvas, 0, 1, 0);
    cairo_fill (canvas);
    cairo_stroke (canvas);
}


gboolean on_draw_event(GtkWidget *widget, cairo_t * contexte, gpointer user_data)
{
    canvas = contexte;
    dessiner();
    return FALSE;
}

GtkWidget *toile;

int main(int argc, char *argv[])
{
    GtkWidget *fenetre;
    gtk_init(&argc, &argv);
    fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position         (GTK_WINDOW(fenetre), GTK_WIN_POS_CENTER);     
    gtk_window_set_default_size     (GTK_WINDOW(fenetre), 800, 600);
    gtk_window_set_title            (GTK_WINDOW(fenetre), "Pacman GTK Cairo");
  
  
    g_signal_connect(G_OBJECT(fenetre), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    toile = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(fenetre), toile);
    g_signal_connect(G_OBJECT(toile), "draw", G_CALLBACK(on_draw_event), NULL);
    gtk_widget_show_all (fenetre);
    gtk_main();
}