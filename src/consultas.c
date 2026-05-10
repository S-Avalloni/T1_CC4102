#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../include/rtree.h"



int main() {
  
  char *rn = "./vectores/v_random_nx.bin";
  char *rs = "./vectores/v_random_st.bin";
  char *en = "./vectores/v_europa_nx.bin";
  char *es = "./vectores/v_europa_st.bin";

  char *nombres[4] = {rn, rs, en, es};
  FILE *archivos[4];

  for (int i = 0; i<4; i++) {
    archivos[i] = fopen(nombres[i], "rb");
    if (archivos[i] == NULL) {
      fprintf(stderr, "Error abriendo archivo %s\n", nombres[i]);
      return 1;
    }
  }

  float s[] = {0.0025, 0.005, 0.01, 0.025, 0.05};
  
  srand(time(NULL));
  
  // coordenadas de x1 e y1 del cuadrado de largo s
  float pos_x, pos_y;
  
  float *lista_x = malloc(sizeof(float)*100);
  float *lista_y = malloc(sizeof(float)*100);
  
  for (int i = 0; i<5; i++) {
    
    printf("\nAnalisis cuadrados aleatorios de tamaño %lf\n",s[i]);
    double esperado = (double)s[i]*(double)s[i]*(double)(1<<24);
    
    for (int j = 0; j<100; j++) {
      do {
        pos_x = (float)rand()/(float)RAND_MAX;
        pos_y = (float)rand()/(float)RAND_MAX;
      } while ((pos_x + s[i] > 1) || (pos_y + s[i] > 1));
      lista_x[j] = pos_x;
      lista_y[j] = pos_y;
    }
    
    for (int f = 0; f<4; f++) { // archivo[f]
      printf("Vector de nodos: %s\n", nombres[f]);
      long total_puntos = 0;
      long lecturas_totales = 0;
      double suma_cuadrado = 0;

      for (int j = 0; j<100; j++) {
  
        float cuadrado[4] = {lista_x[j], lista_x[j]+s[i], lista_y[j], lista_y[j]+s[i]};
        
        long resultado = consultaRectangulo(cuadrado, 0, archivos[f], &lecturas_totales);
        
        total_puntos += resultado;
        suma_cuadrado += ((double)resultado-esperado)*((double)resultado-esperado);
  
      }
  
      printf("promedio puntos encontrados: %f\nvalor esperado de puntos: %f\n", (float)total_puntos/100., esperado);
      printf("desviación estandar puntos: %lf\n", sqrt(suma_cuadrado/99.));
      printf("promedio lecturas de disco: %f\n", (float)lecturas_totales/100.);
    }


  }

  free(lista_x);
  free(lista_y);
  
  return 0;
}

