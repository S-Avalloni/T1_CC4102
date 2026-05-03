

#define b 204 // elementos por bloque

typedef struct {
  float clave[4];
  int valor;
} Nodo;

typedef struct {
  int k; // cantidad de hijos, entre 1 y b
  Nodo hijos[b]; // solo están definidos los primeros k hijos
  char pad[12]; // 12 bytes libres
} Rtree;

