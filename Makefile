hello:
	echo "hello world"


t1: outputs/t1

outputs/t1: src/t1.c include/rtree.h
	gcc -fsanitize=undefined,address -g -Wall -Wextra -std=c11 src/t1.c -o outputs/t1

run: outputs/t1
	./outputs/t1

clean-v:
	rm -f vectores/*