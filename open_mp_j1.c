#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

int main (int argc, char *argv[]) {
	int size = 200000;
	int dimension = 3;
	int bpn = 8;
	int max_dist = 3465;
	FILE *fp = fopen("data.txt", "r");
	int block_end;
	int *histogram = (int*) malloc(sizeof(int)*max_dist);
	for (size_t i = 0; i < max_dist; i++)
		histogram[i] = 0;
	short int *start_points = (short int*) malloc(sizeof(short int)*dimension*size);
	short int *end_points = (short int*) malloc(sizeof(short int)*dimension*size);
	char *buffer = (char*) malloc(bpn*dimension*size);
	float dist;
	omp_set_num_threads(10);
	int nr_read_start;
	int nr_read_end;

	struct timespec start_time;
	struct timespec end_time;
	double time;
	timespec_get(&start_time, TIME_UTC);	

	do {
		nr_read_start = fread(buffer, 1, bpn*size*dimension, fp);
		#pragma omp parallel for
		for (size_t i = 0; i < nr_read_start/bpn; i++) {
			start_points[i] = (buffer[bpn*i+1]-'0')*10000+(buffer[bpn*i+2]-'0')*1000+
									(buffer[bpn*i+4]-'0')*100+(buffer[bpn*i+5]-'0')*10+buffer[bpn*i+6]-'0';
			if (buffer[bpn*i] == '-')
				start_points[i] *= -1;
		}
		#pragma omp parallel for shared (histogram)
		for (size_t i = 0; i < nr_read_start/bpn; i+=dimension) {
			for (size_t j = i+dimension; j < nr_read_start/bpn; j+=dimension) {
				dist = sqrtf((float)((start_points[i]-start_points[j])*(start_points[i]-
						 start_points[j])+(start_points[i+1]-start_points[j+1])*(start_points[i+1]
						 -start_points[j+1])+(start_points[i+2]-start_points[j+2])*(start_points[i+2]
						 -start_points[j+2]))/1000000);
				#pragma omp atomic
				histogram[(int)(dist*100)] += 1; // collisions highly unlikely
			}
		}

		block_end = ftell(fp);

		do {
			nr_read_end = fread(buffer, 1, bpn*size*dimension, fp);
			#pragma omp parallel for
			for (size_t i = 0; i < nr_read_end/bpn; i++) {
				end_points[i] = (buffer[bpn*i+1]-'0')*10000+(buffer[bpn*i+2]-'0')*1000+
									 (buffer[bpn*i+4]-'0')*100+(buffer[bpn*i+5]-'0')*10+
									  buffer[bpn*i+6]-'0';
				if (buffer[bpn*i] == '-')
					end_points[i] *= -1;
			}
			#pragma omp parallel for shared (histogram)
			for (size_t i = 0; i < nr_read_start/bpn; i+=dimension) {
				for (size_t j = 0; j < nr_read_end/bpn; j+=dimension) {
					dist = sqrtf((float)((start_points[i]-end_points[j])*(start_points[i]-
							 end_points[j])+(start_points[i+1]-end_points[j+1])*(start_points[i+1]
							 -end_points[j+1])+(start_points[i+2]-end_points[j+2])*(start_points[i+2]
							 -end_points[j+2]))/1000000);
					#pragma omp atomic
					histogram[(int)(dist*100)] += 1; // collisions highly unlikely
				}
			}
		} while (nr_read_end == size*dimension*bpn);

		fseek(fp, block_end, SEEK_SET);
	} while (nr_read_start == size*dimension*bpn);

	timespec_get(&end_time, TIME_UTC);
	time = difftime(end_time.tv_sec, start_time.tv_sec) +
			 (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.;

	int sum = 0;
	for (size_t i = 0; i <= max_dist; i++)
		if (histogram[i] != 0) {
			sum += histogram[i];
			printf("%02d.%02d %d\n", i/100, i%100, histogram[i]);
		}
	printf("Runtime: %f, %d\n", time, sum);
	
	fclose(fp);
	free(histogram);
	free(start_points);
	free(end_points);
	return 0;
}
