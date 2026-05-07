#include <stdlib.h>
#include <stdio.h>
#include "../include/rtree.h"

int main() {
  // aquí voy a asumir que los arboles ya fueron creados
  // con make vec

  FILE *datos_random = fopen("./datos/random.bin", "rb");
  if (datos_random == NULL) {
    perror("Error abriendo datos\n");
    return 1;
  }


  FILE *vec_test = fopen("./vectores/v_test.bin", "wb");
  if (vec_test == NULL) {
    perror("Error abriendo el archivo de vector\n");
    return 1;
  }

  unsigned long N = 800;
  bulkLoading(N, datos_random, vec_test, nearestX);

  fclose(vec_test);
  return 0;
}