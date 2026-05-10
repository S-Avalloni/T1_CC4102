#ifndef RTREE
#define RTREE

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// funciones max y min para conveniencia
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

// definición de b como constante
#define b 204 // elementos por bloque

// Un par clave-valor
typedef struct {
  // (x1, x2, y1, y2) minimo y maximo de x e y respectivamente
  float clave[4];
  
  // posición relativa en el vector de nodos de este nodo
  int valor;
} Par;

// Firma de las funciones para hacer bulk loading
typedef void (*BulkFunction)(long n, Par pares[], FILE *file);

// Un nodo del Rtree
typedef struct {
  // cantidad de hijos, entre 1 y b
  int k;

  // solo están definidos los primeros k hijos, luego de eso es UB
  Par hijos[b];
  
  // 12 bytes libres, contenido es UB
  char pad[12];
} Rtree;

// Función retorna el menor k tal que k*k >= y
long isqrt(long y);

// e1/e2: Puntero a un Par clave-valor
// Esta función compara dos pares según la coordenada x del centro de su clave, retorna 1 si es que
// e1 > e2, -1 si es que e2 es mayor y 0 si es que son iguales
// Firma de esta manera para que funcione con qsort
int cmp_cx(const void *e1, const void *e2);

// e1/e2: Puntero a un Par clave-valor
// Esta función compara dos pares según la coordenada y del centro de su clave, retorna 1 si es que
// e1 > e2, -1 si es que e2 es mayor y 0 si es que son iguales
// Firma de esta manera para que funcione con qsort
int cmp_cy(const void *e1, const void *e2);

// n: Cantidad de elementos a insertar de la lista pares
// pares: Lista con pares clave-valor a insertar, esta lista debe contener por lo menos n elementos
// file: archivo en el que se insertarán los nodos del rtree
// Esta función inserta nodos del R-tree según la especificación dada de nearestX, dividiendo la lista
// en n/b segmentos horizontales y luego repitiendo recursivamente si es que estos son mas de b segmentos
void nearestX(long n, Par pares[], FILE *file);

// n: Cantidad de elementos a insertar de la lista pares
// pares: Lista con pares clave-valor a insertar, esta lista debe contener por lo menos n elementos
// file: archivo en el que se insertarán los nodos del rtree
// Esta función inserta nodos del R-tree según la especificación dada de sortTileRecursive, dividiendo la lista
// en raiz(n/b) segmentos horizontales y luego raiz(n/b) segmentos verticales para cada uno de estos y
// repitiendo recursivamente si es que estos son mas de b segmentos totales
void sortTileRecursive(long n, Par pares[], FILE *file);

// N: Cantidad de elementos a cargar (pares de floats)
// infile: archivo abierto desde el que se leen los elementos, el archivo debe contener por lo menos 2*N floats
// outfile: archivo abierto al que se le escribe en bits los nodos del rtree
// bulkFunction: Función a utilizar en la carga de los datos (nearestX / sortTileRecursive)
// Esta función espera la cantidad de elementos a leer desde el archivo de entrada y los pone en
// el archivo de salida según el metodo de BulkLoading especificado, estos dos archivos deben 
// estar abiertos al momento de comenzar la función y no los cierra
int bulkLoading(unsigned long N, FILE *infile, FILE* outfile, BulkFunction bulkFunction);


// count: contador que aumenta en 1
// infile: archivo abierto del que se lee el nodo
// offset: posición del nodo (en relación al tamaño de un nodo) 
//         desde el inicio (offset 1 está a sizeof(Rtree) bytes)
// Retorna el nodo del Rtree en infile en la posición offset y 
// aumenta el contador.
// El archivo vuelve a la posición en la que estaba originalmente
Rtree readNode(long *count, FILE *infile, size_t offset);

// cuadrado: (x1, x2, y1, y2) minimo y maximo de x e y respectivamente
// offset: posición de la raiz relativa
// infile: archivo ya abierto desde el que se leen los nodos del R-tree
// lecturas: un contador que aumenta en la cantidad de lecturas que se hicieron al disco
// Esta función retorna la cantidad de leementos
long consultaRectangulo(float cuadrado[4], int offset, FILE *infile, long *lecturas);

#endif