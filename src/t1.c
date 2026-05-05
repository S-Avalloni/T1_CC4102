#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../include/rtree.h"


int main() {
  FILE *datos_rand = fopen("./datos/random.bin", "rb"); 
  if (datos_rand==NULL) {
    perror("Error abriendo el archivo de datos aleatorios\n");
    return 1;
  }

  for(int i = 15; i<=24; i++) {
    remove("./vectores/vec_nodos_rand.bin");


    FILE *vector = fopen("./vectores/vec_nodos_rand.bin", "w");
    if (vector==NULL) {
      perror("Error abriendo el archivo de salida rand NX\n");
      return 1;
    }

    unsigned long N = (unsigned long)1<<i;


    clock_t start, end;
    
    
    double cpu_time_used;
    
    start = clock();
    bulkLoading(N, datos_rand, vector, nearestX);
    end = clock();

    fclose(vector);
    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tomó %f segundos en ejecutarse para N=2^%d\n", cpu_time_used, i);
  }
  
  return 0;
}

