#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../include/rtree.h"


int timeTest(const char *infile, const char *outfile, BulkFunction bulkFunction) {

  FILE *datos = fopen(infile, "rb"); 
  if (datos==NULL) {
    perror("Error abriendo el archivo de datos\n");
    return 1;
  }

  for(int i = 15; i<=24; i++) {
    remove(outfile);


    FILE *vector = fopen(outfile, "wb");
    if (vector==NULL) {
      perror("Error abriendo el archivo de salida\n");
      return 1;
    }

    unsigned long N = (unsigned long)1<<i;


    clock_t start, end;
    
    
    double cpu_time_used;
    
    start = clock();
    bulkLoading(N, datos, vector, bulkFunction);
    end = clock();

    fclose(vector);
    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tomó %f segundos en ejecutarse para N=2^%d\n", cpu_time_used, i);
    fseek(datos, 0, SEEK_SET);
  }
  
  fclose(datos);

  return 0;

}


int main() {
  
  const char *datos_random = "./datos/random.bin";
  const char *datos_eur = "./datos/europa.bin";

  const char *vector_ran_nx = "./vectores/v_random_nx.bin";
  const char *vector_eur_nx = "./vectores/v_europa_nx.bin";
  const char *vector_ran_st = "./vectores/v_random_st.bin";
  const char *vector_eur_st = "./vectores/v_europa_st.bin";
  
  
  printf("\nTest para datos random y NearestX\n");
  timeTest(datos_random, vector_ran_nx, nearestX);

  printf("\nTest para datos europa y NearestX\n");
  timeTest(datos_eur, vector_eur_nx, nearestX);

  printf("\nTest para datos random y SortTileRecursive\n");
  timeTest(datos_random, vector_ran_st, sortTileRecursive);

  printf("\nTest para datos europa y SortTileRecursive\n");
  timeTest(datos_eur, vector_eur_st, sortTileRecursive);

  return 0;
}

