test : test1.c
	gcc -lm -fopenmp -O2 test1.c -o main
.PHONY : clean
clean :
	rm -R distances
	rm -R extracted
