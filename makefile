cell_distances : open_mp.c
	gcc -lm -fopenmp -O2 open_mp.c -o cell_distances
clean :
	rm -R distances
	rm -R extracted
