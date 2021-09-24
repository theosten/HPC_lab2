cell_distances : open_mp.c
	gcc -lm -fopenmp -O2 open_mp.c -o cell_distances
clean :
	rm cell_distances
	rm -R distances
	rm -R extracted
