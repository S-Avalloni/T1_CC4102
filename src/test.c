#include <stdlib.h>
#include <stdio.h>
#include "../include/rtree.h"

int main() {
  // aquí voy a asumir que los arboles ya fueron creados
  // con make vec

  FILE *vec = fopen("./vectores/v_random_nx.bin", "rb");
  if (vec == NULL) {
    perror("Error abriendo el archivo de vector\n");
    return 1;
  }

  Rtree nodo;
  Par hijo;
  hijo.valor = 0;
  long count = 0;
  do {

    nodo = readNode(&count, vec, hijo.valor);
    hijo = nodo.hijos[nodo.k-1];

  } while (hijo.valor != -1);

  printf("%f %f en %ld iteraciones\n", hijo.clave[0], hijo.clave[1], count);

  fclose(vec);
  return 0;
}