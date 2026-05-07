flags = -fsanitize=undefined,address -g -Wall -Wextra -std=c11

say:
	cat Makefile

run-time: outputs/test_time
	outputs/test_time

run-vectores: outputs/create_vec
	outputs/create_vec

run-consultas: outputs/consultas
	outputs/consultas

consultas: outputs/consultas

time: outputs/test_time

vectores: outputs/create_vec

outputs/test_time: src/test_time.c include/rtree.h
	gcc $(flags) src/test_time.c -o outputs/test_time

outputs/create_vec: src/create_vec.c include/rtree.h
	gcc $(flags) src/create_vec.c -o outputs/create_vec

outputs/consultas: src/consultas.c include/rtree.h
	gcc $(flags) src/consultas.c -o outputs/consultas

clean-v:
	rm -f vectores/*
