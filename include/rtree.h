#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define b 204 // elementos por bloque


typedef struct {
  float clave[4];
  int valor;
} Nodo;

typedef void (*BulkFunction)(long n, Nodo pares[], FILE *file);

typedef struct {
  int k; // cantidad de hijos, entre 1 y b
  Nodo hijos[b]; // solo están definidos los primeros k hijos
  char pad[12]; // 12 bytes libres
} Rtree;

// Menor k tal que k*k >= y
long isqrt(long y) {
  long L = 0;
  long R = y + 1;
  long M;
  while (L != R - 1) {
    M = (L + R)>>1;
    if (M * M < y){
      L = M;
    } else {
      R = M;
    }
  }
  return R;
}

int cmp_cx(const void *e1, const void *e2) {
    Nodo *elemA = (Nodo *)e1;
    float v1 = elemA->clave[0] + (elemA->clave[1] - elemA->clave[0])/2.0F;
    
    Nodo *elemB = (Nodo *)e2;
    float v2 = elemB->clave[0] + (elemB->clave[1] - elemB->clave[0])/2.0F;

    if (v1 > v2) {
      return 1;
    } else if (v2 > v1) {
      return -1;
    } else {
      return 0;
    }
}

int cmp_cy(const void *e1, const void *e2) {
    Nodo *elemA = (Nodo *)e1;
    float v1 = elemA->clave[2] + (elemA->clave[3] - elemA->clave[2])/2.0F;
    
    Nodo *elemB = (Nodo *)e2;
    float v2 = elemB->clave[2] + (elemB->clave[3] - elemB->clave[2])/2.0F;

    if (v1 > v2) {
      return 1;
    } else if (v2 > v1) {
      return -1;
    } else {
      return 0;
    }
}

// tomé los n pares llave valor directamente como un nodo
// se especifica que las llaves de los nodos hoja es -1
void nearestX(long n, Nodo pares[], FILE *file) {
  
  // posición actual del archivo
  long curr = ftell(file);
  // en el primer caso se salta el espacio de lo que sería la raiz
  if (curr==0) {
    fseek(file, sizeof(Rtree), SEEK_CUR);
  }

  // se ordena in place con quick sort
  qsort(pares, n, sizeof(Nodo), cmp_cx); 
  Nodo *original = pares;

  Nodo nodo;

  // Un nodo del R-tree
  Rtree rnodo;

  // cantidad de elementos de ''pares'' que ha sido vista
  int count =  0;
  long grupo;
  for(grupo = 0; grupo<(1+(n-1)/b); grupo++){
    rnodo.k = 0;

    // Para calcular el MBR durante el proceso de relleno del
    // nodo del R-tree
    float mbr_x_max, mbr_x_min, mbr_y_max, mbr_y_min;
    mbr_x_max = pares->clave[0];
    mbr_x_min = pares->clave[1];
    mbr_y_max = pares->clave[2];
    mbr_y_min = pares->clave[3];
        
    for(long elemento=0;elemento<b;elemento++){
      if (count >= n) { 
        break;
      }
      count++;

      mbr_x_min = min(mbr_x_min, pares->clave[0]);
      mbr_x_max = max(mbr_x_max, pares->clave[1]);
      mbr_y_min = min(mbr_y_min, pares->clave[2]);
      mbr_y_max = max(mbr_y_max, pares->clave[3]);
      
      rnodo.hijos[rnodo.k] = *pares;
      rnodo.k += 1;

      pares++;
    
    }

    
    nodo.valor = curr/sizeof(Rtree)+grupo+1;
    nodo.clave[0] = mbr_x_min;
    nodo.clave[1] = mbr_x_max;
    nodo.clave[2] = mbr_y_min;
    nodo.clave[3] = mbr_y_max;

    original[grupo] = nodo;
    fwrite(&rnodo, sizeof(Rtree), 1, file); // ver como es que se guardaba, asumo que esto funciona, después lo arreglo
    
    if (count>=n) {break;}
  }
  pares = original;
  if (grupo<=b) {
    rnodo.k = 0;
    for(long i = 0; i<grupo;i++) {
      
      rnodo.hijos[rnodo.k] = *pares;
      rnodo.k += 1;
      pares++;
    }
    fseek(file, 0, SEEK_SET);
    fwrite(&rnodo, sizeof(Rtree), 1, file);
    return;
  };
  
  pares = original;
  nearestX(grupo, pares, file);
}

void sortTileRecursive(long n, Nodo pares[], FILE *file) {
  
  
  // posición actual del archivo
  long curr = ftell(file);
  // en el primer caso se salta el espacio de lo que sería la raiz
  if (curr==0) {
    fseek(file, sizeof(Rtree), SEEK_CUR);
  }

  // se ordena in place con quick sort
  qsort(pares, n, sizeof(Nodo), cmp_cx);
  
  Nodo *original = pares;

  // puede ser que el ultimo de estos grupos tenga muchos menos elementos
  long cantidad_grupos = isqrt(1+(n-1)/b);

  // cantidad maxima de elementos por grupo (que todavía no pueden formar un nodo)
  long limite_grupos = 1+(n-1)/cantidad_grupos;

  long restantes = n;
  
  Nodo nodo;

  // Un nodo del R-tree
  Rtree rnodo;
  long count;

  long grupo;
  for (grupo = 0; grupo<cantidad_grupos; grupo++) {
    long segmento = min(restantes, limite_grupos);

    // se ordenan los elementos que se puedan en su coordenada y
    qsort(pares, segmento, sizeof(Nodo), cmp_cy);
    restantes -= segmento;
    
    float mbr_x_max, mbr_x_min, mbr_y_max, mbr_y_min;
    mbr_x_max = pares->clave[0];
    mbr_x_min = pares->clave[1];
    mbr_y_max = pares->clave[2];
    mbr_y_min = pares->clave[3];

    count = 0;
    rnodo.k = 0; 
    for(long elemento=0;elemento<b;elemento++){
      if (count >= segmento) { 
        break;
      }
      count++;

      mbr_x_min = min(mbr_x_min, pares->clave[0]);
      mbr_x_max = max(mbr_x_max, pares->clave[1]);
      mbr_y_min = min(mbr_y_min, pares->clave[2]);
      mbr_y_max = max(mbr_y_max, pares->clave[3]);
      
      rnodo.hijos[rnodo.k] = *pares;
      rnodo.k += 1;

      pares++;
    
    } // aquí pares termina en el primer lugar no sorteado

    nodo.valor = curr/sizeof(Rtree)+grupo+1;
    nodo.clave[0] = mbr_x_min;
    nodo.clave[1] = mbr_x_max;
    nodo.clave[2] = mbr_y_min;
    nodo.clave[3] = mbr_y_max;

    original[grupo] = nodo;
    fwrite(&rnodo, sizeof(Rtree), 1, file); // ver como es que se guardaba, asumo que esto funciona, después lo arreglo
    
    if (count>=n) {break;}
  }

  pares = original;
  if (grupo<=b) {
    rnodo.k = 0;
    for(long i = 0; i<grupo;i++) {
      
      rnodo.hijos[rnodo.k] = *pares;
      rnodo.k += 1;
      pares++;
    }
    fseek(file, 0, SEEK_SET);
    fwrite(&rnodo, sizeof(Rtree), 1, file);
    return;
  }

  pares = original;
  sortTileRecursive(grupo, pares, file);
}

// Esta función espera la cantidad de elementos a leer desde el archivo de entrada y los pone en
// el archivo de salida según el metodo de BulkLoading especificado, estos dos archivos deben 
// estar abiertos al momento de comenzar la función y no los cierra
int bulkLoading(unsigned long N, FILE *infile, FILE* outfile, BulkFunction bulkFunction) {

  // dos float por cada par
  float *lista = malloc(sizeof(float)*2*N);

  size_t resultado = fread(lista, sizeof(float), 2*N, infile);

  if(resultado != 2*N) {
    fprintf(stderr, "Error leyendo el archivo de datos, %zu, error: %d, eof: %d\n", resultado, ferror(infile), feof(infile));    
    free(lista);
    lista=NULL;
    
    return 1;

  } else {
    printf("Archivo datos abierto y lista llenada, %zu elementos leidos\n", resultado);
  }

  // pares tiene asignado una posición de memoria de tamaño sizeof(Nodo) por cada par de resultados
  Nodo *pares = malloc(sizeof(Nodo)*resultado>>1);

  for (unsigned long i = 0; i<(resultado>>1); i++) {
    pares->clave[1] = lista[2*i];
    pares->clave[0] = lista[2*i]; 
    
    pares->clave[2] = lista[2*i+1]; 
    pares->clave[3] = lista[2*i+1];

    pares->valor = -1;
    pares++;
  }

  pares -= resultado>>1;

  free(lista);
  lista = NULL;

  bulkFunction(resultado>>1, pares, outfile);
  free(pares);
  pares=NULL;
  return 0;
}


