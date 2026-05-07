#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include "../include/rtree.h"

int main() {
  
  FILE *datos_random = fopen("./datos/random.bin", "rb");
  if (datos_random == NULL) {
    perror("Error abriendo el archivo datos random\n");
    return 1;
  }
  FILE *datos_eur = fopen("./datos/europa.bin", "rb");
  if (datos_eur == NULL) {
    perror("Error abriendo el archivo datos europa\n");
    return 1;
  }

  remove("./vectores/v_random_nx.bin");
  remove("./vectores/v_europa_nx.bin");
  remove("./vectores/v_random_st.bin");
  remove("./vectores/v_europa_st.bin");

  FILE *vector_ran_nx = fopen("./vectores/v_random_nx.bin", "wb");
  if (vector_ran_nx == NULL) {
    perror("Error abriendo el archivo ran_nx\n");
    return 1;
  }

  FILE *vector_eur_nx = fopen("./vectores/v_europa_nx.bin", "wb");
  if (vector_eur_nx == NULL) {
    perror("Error abriendo el archivo eur_nx\n");
    return 1;
  }

  FILE *vector_ran_st = fopen("./vectores/v_random_st.bin", "wb");
  if (vector_ran_st == NULL) {
    perror("Error abriendo el archivo ran_st\n");
    return 1;
  }

  FILE *vector_eur_st = fopen("./vectores/v_europa_st.bin", "wb");
  if (vector_eur_st == NULL) {
    perror("Error abriendo el archivo eur_st\n");
    return 1;
  }

  long N = 1<<24;


  bulkLoading(N, datos_random, vector_ran_nx, nearestX);
  printf("vector_ran_nx creado\n");
  fseek(datos_random, 0, SEEK_SET);
  bulkLoading(N, datos_random, vector_ran_st, sortTileRecursive);
  printf("vector_ran_st creado\n");

  bulkLoading(N, datos_eur, vector_eur_st, sortTileRecursive);
  printf("vector_eur_st creado\n");
  fseek(datos_eur, 0, SEEK_SET);
  bulkLoading(N, datos_eur, vector_eur_nx, nearestX);
  printf("vector_eur_nx creado\n");

  fclose(datos_random);
  fclose(datos_eur);
  fclose(vector_ran_nx);
  fclose(vector_eur_nx);
  fclose(vector_ran_st);
  fclose(vector_eur_st);

  return 0;
}

