#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include "../include/rtree.h"


int main() {
  FILE *datos = fopen("./datos/random.bin", "rb"); 
  if (datos==NULL) {
    perror("Error abriendo el archivo de datos");
    return 1;
  }

  FILE *vector = fopen("./vectores/vec_nodos_rand.bin", "w");
  if (vector==NULL) {
    perror("Error abriendo el archivo de salida");
    return 1;
  }
  

  unsigned long N = 1<<15;
  bulkLoading(N, datos, vector, nearestX);
  
  return 0;
}

