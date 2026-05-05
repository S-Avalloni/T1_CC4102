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

  unsigned long N = 1<<15;
  float *lista = malloc(sizeof(float)*2*N);

  if (lista==NULL) {
    perror("ya no se\n");
  }

  size_t resultado = fread(lista, sizeof(float), 2*N, datos);

  if(resultado != 2*N) {
    fprintf(stderr, "Error leyendo el archivo de datos, %zu, error: %d, eof: %d\n", resultado, ferror(datos), feof(datos));
    
    // fprintf(stderr, "Error leyendo el archivo de datos, %lu, feof: %d, ferr: %d\n", resultado, feof(datos), ferror(datos));
    
    fclose(datos);

    free(lista);
    lista=NULL;
    
    return 1;
  } else {
    printf("Archivo datos abierto y lista llenada, %zu elementos leidos\n", resultado);
  }

  fclose(datos);

  Nodo *pares = malloc(sizeof(Nodo)*resultado>>2);

  for (unsigned long i = 0; i<resultado>>2; i++) {
    pares->clave[1] = lista[2*i];
    pares->clave[0] = lista[2*i]; 
    
    pares->clave[2] = lista[2*i+1]; 
    pares->clave[3] = lista[2*i+1];

    pares->valor = -1;
    pares++;
  }

  pares -= resultado>>2;

  free(lista);
  lista = NULL;


  FILE *vec_nodos_aleatorio = fopen("./vectores/vec_nodos_rand.bin", "wb+");
  if (vec_nodos_aleatorio==NULL) {
    perror("Error abriendo el archivo de vector de nodos");
    fclose(vec_nodos_aleatorio);
    free(pares);
    pares=NULL;
    return 1;
  } else {
    printf("Arvhivo vector de nodos abierto\n");
  }

  nearestX(resultado>>2, pares, vec_nodos_aleatorio);
  return 0;
}

