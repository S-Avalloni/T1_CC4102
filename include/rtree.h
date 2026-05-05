#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define b 204 // elementos por bloque

typedef void (*BulkFunction)(long n, Nodo pares[], FILE *file);

typedef struct {
  float clave[4];
  int valor;
} Nodo;

typedef struct {
  int k; // cantidad de hijos, entre 1 y b
  Nodo hijos[b]; // solo están definidos los primeros k hijos
  char pad[12]; // 12 bytes libres
} Rtree;

// Extraido de wikipedia https://en.wikipedia.org/wiki/Integer_square_root#Algorithm_using_binary_search
long isqrt(long y) {
  long L = 0;
  long R = y + 1;
  long M;
  while (L != R - 1) {
    M = (L + R);
    if (M * M <= y){
      L = M;
    } else {
      R = M;
    }
  }
  return L;
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
      Nodo hijo = {  
        .clave = {pares->clave[0], pares->clave[1], pares->clave[2], pares->clave[3]},
        .valor = pares->valor
      };

      mbr_x_min = min(mbr_x_min, pares->clave[0]);
      mbr_x_max = max(mbr_x_max, pares->clave[1]);
      mbr_y_min = min(mbr_y_min, pares->clave[2]);
      mbr_y_max = max(mbr_y_max, pares->clave[3]);
      
      rnodo.hijos[rnodo.k] = hijo;
      rnodo.k++;

      pares++;
    
    }

    
    nodo.valor = curr/sizeof(Rtree)+grupo;
    nodo.clave[0] = mbr_x_min;
    nodo.clave[1] = mbr_x_max;
    nodo.clave[2] = mbr_y_min;
    nodo.clave[3] = mbr_y_max;

    pares[grupo] = nodo;
    fwrite(&rnodo, sizeof(Rtree), 1, file); // ver como es que se guardaba, asumo que esto funciona, después lo arreglo
    
    if (count>=n) {break;}
  }

  if (grupo<=b) {
    for(long i = 0; i<grupo;i++) {
      pares++;
      Nodo hijo = {
        .clave = {pares->clave[0], pares->clave[1], pares->clave[2], pares->clave[3]},
        .valor = pares->valor
      };
      rnodo.hijos[rnodo.k] = hijo;
      rnodo.k++;
    }
    fseek(file, 0, SEEK_SET);
    fwrite(&rnodo, sizeof(Rtree), 1, file);
    return;
  };
  nearestX(n, pares, file);
}

void sortTileRecursive(long n, Nodo pares[], FILE *file) {
  
  long curr = ftell(file);
  if (curr==0) {
    fseek(file, sizeof(Rtree), SEEK_CUR);
  }

  qsort(pares, n, sizeof(Nodo), cmp_cx); // se ordena in place con quick sort
  long tamaño = isqrt((double)n/(double)b); // debo definirlo, no se si es que está bien, deberíamos tener grupos de b elementos o menos, no se puedenn mas, hay que fijarse que la multiplicación de estos numeros sea correcta
  for (;;) { // aquí debería seguir hasta que tamaño sea mas grande que lo que quede de pares y luego hacer un ultimo sort con lo ultimo
    qsort(pares, tamaño, sizeof(Nodo), cmp_cy);
    pares+=tamaño;
  }

  Nodo *nodos = malloc(sizeof(Nodo)*(1+(n-1)/b));
  Rtree rnodo;
  int count =  0;
  long grupo;
  for(grupo = 0; grupo<(1+(n-1)/b); grupo++){
    rnodo.k = 0;
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
      Nodo nodo = {  
        .clave = {pares->clave[0], pares->clave[1], pares->clave[2], pares->clave[3]},
        .valor = pares->valor
      };

      mbr_x_min = min(mbr_x_min, pares->clave[0]);
      mbr_x_max = max(mbr_x_max, pares->clave[1]);
      mbr_y_min = min(mbr_y_min, pares->clave[2]);
      mbr_y_max = max(mbr_y_max, pares->clave[3]);
      
      rnodo.hijos[rnodo.k] = nodo;
      rnodo.k++;

      pares++;
    
    }


    nodos->valor = curr/sizeof(Rtree)+grupo;
    nodos->clave[0] = mbr_x_min;
    nodos->clave[1] = mbr_x_max;
    nodos->clave[2] = mbr_y_min;
    nodos->clave[3] = mbr_y_max;

    pares[grupo] = *nodos;
    fwrite(&rnodo, sizeof(Rtree), 1, file); // ver como es que se guardaba, asumo que esto funciona, después lo arreglo
    
    if (count>=n) {break;}
  }
  if (grupo<=b) {
    for(long i = 0; i<grupo;i++) {
      pares++;
      Nodo nodo = {
        .clave = {pares->clave[0], pares->clave[1], pares->clave[2], pares->clave[3]},
        .valor = pares->valor
      };
      rnodo.hijos[rnodo.k] = nodo;
      rnodo.k++;
    }
  };
  sortTileRecursive(n, pares, file);
}

int bulkLoading(unsigned long N, FILE *infile, FILE* outfile, BulkFunction bulkFunction) {

  float *lista = malloc(sizeof(float)*2*N);

  size_t resultado = fread(lista, sizeof(float), 2*N, infile);

  if(resultado != 2*N) {
    fprintf(stderr, "Error leyendo el archivo de datos, %zu, error: %d, eof: %d\n", resultado, ferror(infile), feof(infile));    
    fclose(infile);

    free(lista);
    lista=NULL;
    
    return 1;

  } else {
    printf("Archivo datos abierto y lista llenada, %zu elementos leidos\n", resultado);
  }

  fclose(infile);

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

  bulkFunction(resultado>>2, pares, outfile);
  return 0;
}