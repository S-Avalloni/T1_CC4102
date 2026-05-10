#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rtree.h"

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
    Par *elemA = (Par *)e1;
    float v1 = elemA->clave[0] + (elemA->clave[1] - elemA->clave[0])/2.0F;
    
    Par *elemB = (Par *)e2;
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
    Par *elemA = (Par *)e1;
    float v1 = elemA->clave[2] + (elemA->clave[3] - elemA->clave[2])/2.0F;
    
    Par *elemB = (Par *)e2;
    float v2 = elemB->clave[2] + (elemB->clave[3] - elemB->clave[2])/2.0F;

    if (v1 > v2) {
      return 1;
    } else if (v2 > v1) {
      return -1;
    } else {
      return 0;
    }
}

// Todo!
void nearestX(long n, Par pares[], FILE *file) {
  
  // posición actual del archivo
  long curr = ftell(file);
  // en el primer caso se salta el espacio de lo que sería la raiz
  if (curr==0) {
    fseek(file, sizeof(Rtree), SEEK_CUR);
    curr = sizeof(Rtree);
  }

  // se ordena in place con quick sort
  qsort(pares, n, sizeof(Par), cmp_cx); 
  Par *original = pares;

  Par par;

  Rtree rnodo;

  // cantidad de elementos de pares que ha sido vista
  int count =  0;

  long grupo = 0;
  do {

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

    
    par.valor = curr/sizeof(Rtree)+grupo;
    par.clave[0] = mbr_x_min;
    par.clave[1] = mbr_x_max;
    par.clave[2] = mbr_y_min;
    par.clave[3] = mbr_y_max;

    original[grupo] = par;
    fwrite(&rnodo, sizeof(Rtree), 1, file);
    grupo++;

  } while (count < n);

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

void sortTileRecursive(long n, Par pares[], FILE *file) {
  
  
  // posición actual del archivo
  long curr = ftell(file);
  // en el primer caso se salta el espacio de lo que sería la raiz
  if (curr==0) {
    fseek(file, sizeof(Rtree), SEEK_CUR);
    curr = sizeof(Rtree);
  }

  // se ordena in place con quick sort
  qsort(pares, n, sizeof(Par), cmp_cx);
  Par *original = pares;

  // puede ser que el ultimo de estos grupos tenga muchos menos elementos
  long cantidad_grupos = isqrt(1+(n-1)/b);

  // cantidad maxima de elementos por grupo (que todavía no pueden formar un nodo)
  long limite_grupos = 1+(n-1)/cantidad_grupos;

  long restantes = n;
  
  Par par;

  // Un nodo del R-tree
  Rtree rnodo;
  
  long count_total = 0;
  long grupo = 0;
  do { // grupo
    // tamaño del segmento a ordenar
    long segmento = min(restantes, limite_grupos);

    // se ordenan los elementos que se puedan en su coordenada y
    qsort(pares, segmento, sizeof(Par), cmp_cy);
    restantes -= segmento;
    
    long count_segment = 0;

    do { // subgrupo

      float mbr_x_max, mbr_x_min, mbr_y_max, mbr_y_min;
      mbr_x_max = pares->clave[0];
      mbr_x_min = pares->clave[1];
      mbr_y_max = pares->clave[2];
      mbr_y_min = pares->clave[3];
  
      rnodo.k = 0; 
      for(long elemento=0;elemento<b;elemento++){
        if (count_segment >= segmento) { 
          break;
        }
        count_segment++;
  
        mbr_x_min = min(mbr_x_min, pares->clave[0]);
        mbr_x_max = max(mbr_x_max, pares->clave[1]);
        mbr_y_min = min(mbr_y_min, pares->clave[2]);
        mbr_y_max = max(mbr_y_max, pares->clave[3]);
        
        rnodo.hijos[rnodo.k] = *pares;
        rnodo.k += 1;
  
        pares++;
      
      }
      par.valor = curr/sizeof(Rtree)+grupo;
      par.clave[0] = mbr_x_min;
      par.clave[1] = mbr_x_max;
      par.clave[2] = mbr_y_min;
      par.clave[3] = mbr_y_max;
  
      original[grupo] = par;
      fwrite(&rnodo, sizeof(Rtree), 1, file); // ver como es que se guardaba, asumo que esto funciona, después lo arreglo
      grupo++;
    } while (count_segment < segmento);
    count_total += count_segment;
  } while (count_total < n);

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

  // pares tiene asignado una posición de memoria de tamaño sizeof(Par) por cada par de resultados
  Par *pares = malloc(sizeof(Par)*N);

  for (unsigned long i = 0; i < N; i++) {
    pares->clave[1] = lista[2*i];
    pares->clave[0] = lista[2*i]; 
    
    pares->clave[2] = lista[2*i+1]; 
    pares->clave[3] = lista[2*i+1];

    pares->valor = -1;
    pares++;
  }

  pares -= N;

  free(lista);
  lista = NULL;

  bulkFunction(N, pares, outfile);
  free(pares);
  pares=NULL;
  return 0;
}


Rtree readNode(long *count, FILE *infile, size_t offset) {
  unsigned long curr = ftell(infile);
  fseek(infile, offset*sizeof(Rtree), SEEK_SET);
  Rtree nodo;
  fread(&nodo, sizeof(Rtree), 1, infile);
  fseek(infile, curr, SEEK_SET);
  *count += 1;
  return nodo;
}

long consultaRectangulo(float rectangulo[4], int offset, FILE *infile, long *lecturas) {

  // voy a asumir que la raiz siempre intersecta
  Rtree nodo = readNode(lecturas, infile, offset);
  
  long conteo = 0;
  for (int i = 0; i<nodo.k; i++) {
    if( (nodo.hijos[i].clave[0] > rectangulo[1]) || // Rtree -> min x > max x <- rectangulo
        (nodo.hijos[i].clave[1] < rectangulo[0]) || //          max x < min x 
        (nodo.hijos[i].clave[2] > rectangulo[3]) || //          min y > max y
        (nodo.hijos[i].clave[3] < rectangulo[2])) { //          max y < max y
      
      // Si se cumple alguna de las condiciones el rectangulo no se intersecta
      // con el nodo, si cualquiera se rompe quiere decir que se intersectan
      // de alguna manera
      continue;
    }
    
    if (nodo.hijos[i].valor == -1) {
      conteo += 1;
      continue;
    }
    
    conteo += consultaRectangulo(rectangulo, nodo.hijos[i].valor, infile, lecturas);
  }

  return conteo;

}


