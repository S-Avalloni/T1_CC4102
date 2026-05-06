hello:
	echo "hello world"



run-time: outputs/test_time
	outputs/test_time

run-vectores: outputs/create_vec
	outputs/create_vec

time: outputs/test_time

vectores: outputs/create_vec

outputs/test_time: src/test_time.c include/rtree.h
	gcc -O2 -fsanitize=undefined,address -g -Wall -Wextra -std=c11 src/test_time.c -o outputs/test_time

clean-v:
	rm -f vectores/*