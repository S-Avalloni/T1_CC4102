#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../include/rtree.h"


// cuadrado: (x1, x2, y1, y2) minimo y maximo de x e y respectivamente
// offset: posición de la raiz relativa
// infile: archivo ya abierto desde el que se leen los nodos del R-tree
long consultaCuadrado(float cuadrado[4], int offset, FILE *infile, long *lecturas) {

  // voy a asumir que la raiz siempre intersecta
  Rtree nodo = readNode(lecturas, infile, offset);
  
  long conteo = 0;
  for (int i = 0; i<nodo.k; i++) {
    if( (nodo.hijos[i].clave[0] > cuadrado[1]) || // Rtree -> min x > max x <- cuadrado
        (nodo.hijos[i].clave[1] < cuadrado[0]) || //          max x < min x 
        (nodo.hijos[i].clave[2] > cuadrado[3]) || //          min y > max y
        (nodo.hijos[i].clave[3] < cuadrado[2])) { //          max y < max y
      
      // Si se cumple alguna de las condiciones el cuadrado no se intersecta
      // con el nodo, si cualquiera se rompe quiere decir que se intersectan
      // de alguna manera
      continue;
    }
    
    if (nodo.hijos[i].valor == -1) {
      conteo += 1;
      continue;
    }
    
    conteo += consultaCuadrado(cuadrado, nodo.hijos[i].valor, infile, lecturas);
  }

  return conteo;

}

int main() {
  
  FILE *rand_nx = fopen("./vectores/v_random_nx.bin", "rb");
  if (rand_nx == NULL) {
    perror("Error leyendo archivo de vector");
    return 1;
  }



  float s[] = {0.0025, 0.005, 0.01, 0.025, 0.05};
  
  srand(time(NULL));
  
  // coordenadas de x1 e y1 del cuadrado de largo s
  float pos_x, pos_y;
  
  float *lista_x = malloc(sizeof(float)*100);
  float *lista_y = malloc(sizeof(float)*100);
  
  for (int i = 0; i<5; i++) {
    long total_puntos = 0;
    long lecturas_totales = 0;
    double suma_cuadrado = 0;
    double esperado = (double)s[i]*(double)s[i]*(double)(1<<24);
    
    for (int j = 0; j<100; j++) {
      do {
        pos_x = (float)rand()/(float)RAND_MAX;
        pos_y = (float)rand()/(float)RAND_MAX;
      } while ((pos_x + s[i] > 1) || (pos_y + s[i] > 1));
      lista_x[j] = pos_x;
      lista_y[j] = pos_y;
    }


    for (int j = 0; j<100; j++) {

      float cuadrado[4] = {lista_x[j], lista_x[j]+s[i], lista_y[j], lista_y[j]+s[i]};
      
      long resultado = consultaCuadrado(cuadrado, 0, rand_nx, &lecturas_totales);
      
      // printf("%ld elementos dentro del cuadrado en %ld lecturas\n", resultado, lecturas_totales);
      total_puntos += resultado;
      suma_cuadrado += ((double)resultado-esperado)*((double)resultado-esperado);

    }

    printf("promedio puntos encontrados: %f\nvalor esperado de puntos: %f\n", (float)total_puntos/100., esperado);
    printf("desviación estandar puntos: %f\n", (float)suma_cuadrado/99.);
    printf("promedio lecturas de disco: %f\n", (float)lecturas_totales/100.);

  }

  free(lista_x);
  free(lista_y);
  
  return 0;
}

