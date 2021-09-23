cell_distances : test1.c
	gcc -lm -fopenmp -O2 -std=c11 test1.c -o cell_distances
clean :
	rm cell_distances
