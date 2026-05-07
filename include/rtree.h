#ifndef RTREE
#define RTREE

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define b 204 // elementos por bloque


typedef struct {
  // (x1, x2, y1, y2) minimo y maximo de x e y respectivamente
  float clave[4];
  
  // posición relativa en el vector de nodos de este nodo
  int valor;
} Par;

// firma de las funciones para hacer bulk loading
typedef void (*BulkFunction)(long n, Par pares[], FILE *file);

// un nodo del Rtree
typedef struct {
  // cantidad de hijos, entre 1 y b
  int k;

  // solo están definidos los primeros k hijos, luego de eso es UB
  Par hijos[b];
  
  // 12 bytes libres, contenido es UB
  char pad[12];
} Rtree;

// Menor k tal que k*k >= y
long isqrt(long y);

int cmp_cx(const void *e1, const void *e2);

int cmp_cy(const void *e1, const void *e2);

// Todo!
void nearestX(long n, Par pares[], FILE *file);

void sortTileRecursive(long n, Par pares[], FILE *file);

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

#endif