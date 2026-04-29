

typedef struct {
  int k; // cantidad de hijos, entre 1 y b
  struct {
    float clave[204][4]; // b claves especificando los limites del MBR
    int valor[204]; // para cada MBR la posición relativa al archivo de ese nodo
  } hijos;
  char pad[12]; // 12 bytes libres
} Nodo;

