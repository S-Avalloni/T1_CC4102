hello:
	echo "hello world"


t1: outputs/t1

outputs/t1: src/t1.c include/rtree.h
	gcc -Wall -Wextra -g -std=c11 src/t1.c -o outputs/t1

run: outputs/t1
	./outputs/t1
