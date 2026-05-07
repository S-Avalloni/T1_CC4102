flags = -fsanitize=undefined,address -g -Wall -Wextra -std=c11
rtree = outputs/rtree.o

say:
	cat Makefile

build: consultas time vectores test $(rtree)

run-time: outputs/test_time
	outputs/test_time

run-vectores: outputs/create_vec
	outputs/create_vec

run-consultas: outputs/consultas
	outputs/consultas

run-test: outputs/test
	outputs/test

consultas: outputs/consultas

time: outputs/test_time

vectores: outputs/create_vec

test: outputs/test

outputs/test_time: src/test_time.c $(rtree)
	gcc $(flags) $(rtree) src/test_time.c -o outputs/test_time

outputs/create_vec: src/create_vec.c $(rtree)
	gcc $(flags) $(rtree) src/create_vec.c -o outputs/create_vec

outputs/consultas: src/consultas.c $(rtree)
	gcc $(flags) $(rtree) src/consultas.c -o outputs/consultas

outputs/test: src/test.c $(rtree)
	gcc $(flags) $(rtree) src/test.c -o outputs/test

$(rtree): include/rtree.h include/rtree.c
	gcc $(flags) -c include/rtree.c -o outputs/rtree.o

clean-v:
	rm -f vectores/*

clean-x:
	rm -f outputs/*