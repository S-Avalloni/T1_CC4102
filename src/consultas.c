#include <stdlib.h>
#include <stdio.h>
#include "../include/rtree.h"

int main() {
  // aquí voy a asumir que los arboles ya fueron creados
  // con make vec

  long count = 0;
  FILE *vec_rand_nx = fopen("./vectores/v_random_nx.bin", "rb");
  if (vec_rand_nx == NULL) {
    perror("Error abriendo el archivo de vector\n");
    return 1;
  }


  Rtree nodo;
  Par hijo;
  hijo.valor = 0;
  do {
    nodo = readNode(&count, vec_rand_nx, hijo.valor);
    printf("%ld\n", count);
    
    hijo = nodo.hijos[0];
  } while (hijo.valor != -1);
  
  printf("%f %f\n", hijo.clave[0], hijo.clave[2]);

  fclose(vec_rand_nx);
  return 0;
}