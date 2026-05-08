# Bulk-loading de R-trees

En esta tarea se hizo el codigo para la carga de datos en un R-tree en memoria, la medición de cuanto demora en relación al tamaño del conjunto de datos y un analisis de los tiempos de busqueda y la cantidad de lecturas de disco de estas dependiendo de la construcción del arbol y el dataset.

## Estructutra del proyecto

```
T1/
│
├── include/
│ ├── rtree.h
│ └── rtree.c
│
├── src/
│ ├── consultas.c
│ ├── create_vec.c
│ ├── test_time.c
│ └── test.c
│
├── datos/ # Carpeta donde deben estar guardados los archivos de datos
│
├── outputs/ # Carpeta en la que quedan los ejecutables y archivos .o
│
├── vectores/ # Carpeta en la que quedan los vectores de nodos generados
│
├── Makefile # Instrucciones para compilar y ejecutar los archivos
└── README.md # Este archivo
```

## Compilación

Para compilar todos los programas se debe ejecutar el comando

```bash
make build
```
Este comando generará 4 ejecutables y un archivo objeto en la carpeta `./outputs/`:

- `create_vec`: Crea 4 archivos de vectores de nodos a partir de los archivos en `./datos/` con _N=2^24_
- `test_time`: Reporta el tiempo de creación de los arboles con las distintas cantidades de datos y ambas funciones de _Bulk loading_
- `consultas`: Hace el analisis de 100 consultas a todos los vectores de nodos ya hechos (Asume que ya están creados)
- `test`: Archivo utilizado para testeo personal

En el caso de querer compilar un ejecutable especifico las opciones que se dan son respectivamente:

```bash
make vectores
make time
make consultas
make test
```

Mientras que para ejecutar cualquiera de estos solo basta agregar `run-` antes del parametro. (Ej. `make run-vectores`)

## Uso

### 1. Construcción los R-trees

#### 1.1. Construcción final

```bash
make run-vectores
```

Este comando crea en `./vectores/` un vector de nodos de _N=2^24_ datos por cada combinación de los archivos de datos y funciones de bulk loading, en nuestro caso datos `random.bin` y `europa.bin` con las funciones `nearestX` y `sortTileRecursive`

* `v_europa_nx.bin`
* `v_europa_st.bin`
* `v_random_nx.bin`
* `v_random_st.bin`

#### 1.2. Reporte de construcción

```bash
make run-time
```

De forma similar al anterior este comando crea los vectores de nodos multiples veces para los _N_ solicitados y para luego reportar sus tiempos de creación. Al finalizar los archivos restantes son los correspondientes a _N=2^24_

### Consultas

```bash
make run-consultas
```

Asumiendo que los R-trees ya están creados este comando realiza el test de las consultas de 100 cuadrados aleatorios de los tamaños señalados en el enunciado de esta tarea para cada uno de los vectores de nodos y luego reporta para cada combinación tamaño-vector

- Promedio de lecturas de disco
- Promedio de puntos encontrados
- Desviación estandar muestral

### Limpieza

```bash
make clean-v
make clean-x
```

Estos archivos vacían las carpetas `./vectores/` y `./outputs/` respectivamente, por si es que se quiere ejecutar desde cero la creación de vectores o compilación

## Miscelaneo

```bash
make say
# o alternativamente
make
```

Muestra en pantalla todo el contenido de `Makefile`