#include "stdio.h"
#include "stdlib.h"
#include <string.h>

typedef struct conf{
  int* taille;
  int temperature_base;
  int seuil;
  int* tmps;
} conf;

int* read_size(FILE * f);
int* read_tmps(int taille[2]);
int read_int(FILE* f);


/*conf read_conf() {
  conf c;
  c.taille = read_size();
  c.temperature_base = read_int();
  c.seuil = read_int();
  c.tmps = read_tmps(c.taille);
  return c;
}*/


int* read_size(FILE* f) {
  static int taille[2];
  double d;
  fscanf(f, "%d,%d", &taille[0], &taille[1]);
 // printf("\n%2f\n", d);
  return taille;
}

int read_int(FILE* f) {
  int i;
  fscanf(f, "%d", &i);
  return i;
}

double read_double(FILE* f) {
  double d;
  fscanf(f, "%lf", &d);
  return d;
}


/*int* read_tmps(int taille[2]) {
  int total = taille[0]*taille[1];
  int* tmps = (int*) malloc(sizeof(int)*total);
  for (int i = 0; i<total; i++) {
    tmps[i] = read_int();
  }
  return tmps;
}*/

/*void print_conf(conf* c) {
  printf("size %d, %d \n seuil %d \n temperature ambiance %d", c->taille[0], c->taille[1], c->seuil, c->temperature_base);
  for (int i = 0; i<c->taille[0]; i++) {
    for (int j = 0; j<c->taille[1]; j++) {
       printf("%f", c->tmps[i*]);
    }
    printf("\n");
  }
}*/