#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

int main (int argc, char *argv[]) {
	int size = 10;
	int dimension = 3;
	int max_dist = 3465;
	FILE *fp = fopen("data.txt", "r");
	int block_end;
	int *histogram = (int*) malloc(sizeof(int)*max_dist);
	for (size_t i = 0; i < max_dist; i++)
		histogram[i] = 0;
	short int *start_points = (short int*) malloc(sizeof(short int)*dimension*size); // <2.5MiB
	short int *end_points = (short int*) malloc(sizeof(short int)*dimension*size); // <2.5MiB
	int length = size*3;
	float dist;
	omp_set_num_threads(4);
	char array[24];
	int done = 0;
	clock_t time = clock();

	while (length == size*3) {
		for (size_t i = 0; i < size*dimension; i+=dimension) {
			if (fread(array, 1, 24, fp) == 24) {
				start_points[i] = (array[1]-'0')*10000+(array[2]-'0')*1000+(array[4]-'0')*100
								 +(array[5]-'0')*10+array[6]-'0';
				if (array[0] == '-')
					start_points[i] *= -1;
				
				start_points[i+1] = (array[9]-'0')*10000+(array[10]-'0')*1000+(array[12]-'0')*100
								 +(array[13]-'0')*10+array[14]-'0';
				if (array[8] == '-')
					start_points[i+1] *= -1;
				
				start_points[i+2] = (array[17]-'0')*10000+(array[18]-'0')*1000+(array[20]-'0')*100
								 +(array[21]-'0')*10+array[22]-'0';
				if (array[16] == '-')
					start_points[i+2] *= -1;
			} else {
				length = i;
				break;
			}
		}
		for (size_t i = 0; i < length; i+=dimension) {
			for (size_t j = i+dimension; j < length; j+=dimension) {
				dist = sqrtf((float)((start_points[i]-start_points[j])*(start_points[i]-
						 start_points[j])+(start_points[i+1]-start_points[j+1])*(start_points[i+1]
						 -start_points[j+1])+(start_points[i+2]-start_points[j+2])*(start_points[i+2]
						 -start_points[j+2]))/1000000);
				histogram[(int)(dist*100)] += 1; // collisions highly unlikely
			}
		}
		block_end = ftell(fp);
		done = size*3;
		while (done == size*3) {
			for (size_t i = 0; i < size*dimension; i+=dimension) {
				if (fread(array, 1, 24, fp) == 24) {
					end_points[i] = (array[1]-'0')*10000+(array[2]-'0')*1000+(array[4]-'0')*100
									 +(array[5]-'0')*10+array[6]-'0';
					if (array[0] == '-')
						end_points[i] *= -1;
				
					end_points[i+1] = (array[9]-'0')*10000+(array[10]-'0')*1000+(array[12]-'0')*100
									 +(array[13]-'0')*10+array[14]-'0';
					if (array[8] == '-')
						end_points[i+1] *= -1;
				
					end_points[i+2] = (array[17]-'0')*10000+(array[18]-'0')*1000+(array[20]-'0')*100
									 +(array[21]-'0')*10+array[22]-'0';
					if (array[16] == '-')
						end_points[i+2] *= -1;
				} else {
					done = i;
					break;
				}
			}
			for (size_t i = 0; i < length; i+=dimension) {
				for (size_t j = 0; j < done; j+=dimension) {
					dist = sqrtf((float)((start_points[i]-end_points[j])*(start_points[i]-
							 end_points[j])+(start_points[i+1]-end_points[j+1])*(start_points[i+1]
							 -end_points[j+1])+(start_points[i+2]-end_points[j+2])*(start_points[i+2]
							 -end_points[j+2]))/1000000);
					histogram[(int)(dist*100)] += 1; // collisions highly unlikely
				}
			}
		}
		fseek(fp, block_end, SEEK_SET);
	}

	double avg_time = (double)(clock() - time) / CLOCKS_PER_SEC;

	for (size_t i = 0; i <= max_dist; i++)
		if (histogram[i] != 0) {
			printf("%02d.%02d %d\n", i/100, i%100, histogram[i]);
		}
	printf("Runtime: %f\n", avg_time);
	
	fclose(fp);
	free(histogram);
	free(start_points);
	free(end_points);
	return 0;
}
