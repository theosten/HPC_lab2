#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
	int opt, t;
	while ((opt = getopt(argc, argv, "t:")) != -1) {
		switch (opt) {
			case 't':
				t = atoi(optarg);
				break;
		}
	}
	int size = 100000;
	int dimension = 3;
	int bpn = 8;
	#define max_dist 3465
	FILE *fp = fopen("cells", "r");
	int block_end;
	int histogram[max_dist] = {0};
	float *start_points = (float*) malloc(sizeof(float)*dimension*size);
	float *end_points = (float*) malloc(sizeof(float)*dimension*size);
	char *buffer = (char*) malloc(bpn*dimension*size);
	omp_set_num_threads(t);
	int nr_read_start;
	int nr_read_end;

	do {
		nr_read_start = fread(buffer, 1, bpn*size*dimension, fp)/bpn;
		for (size_t i = 0; i < nr_read_start; i++) {
			start_points[i] = (buffer[bpn*i+1]-'0')*10.+(buffer[bpn*i+2]-'0')+
									(buffer[bpn*i+4]-'0')/10.+(buffer[bpn*i+5]-'0')/100.+
									(buffer[bpn*i+6]-'0')/1000.;
			if (buffer[bpn*i] == '-')
				start_points[i] *= -1;
		}
		#pragma omp parallel
		{
		int local_histogram[max_dist] = {0};
		float x, y, z;
		#pragma omp for
		for (size_t i = 0; i < nr_read_start; i+=dimension) {
			for (size_t j = i+dimension; j < nr_read_start; j+=dimension) {
				x = start_points[i]-start_points[j];
				y = start_points[i+1]-start_points[j+1];
				z = start_points[i+2]-start_points[j+2];
				local_histogram[(int)(sqrtf(x*x+y*y+z*z)*100)] += 1;
			}
		}
		#pragma omp critical
		{
		for (size_t i = 0; i < max_dist; i++)
			histogram[i] += local_histogram[i];
		}}

		block_end = ftell(fp);

		do {
			nr_read_end = fread(buffer, 1, bpn*size*dimension, fp)/bpn;
			for (size_t i = 0; i < nr_read_end; i++) {
				end_points[i] = (buffer[bpn*i+1]-'0')*10.+(buffer[bpn*i+2]-'0')+
									 (buffer[bpn*i+4]-'0')/10.+(buffer[bpn*i+5]-'0')/100.+
									 (buffer[bpn*i+6]-'0')/1000.;
				if (buffer[bpn*i] == '-')
					end_points[i] *= -1;
			}
			#pragma omp parallel
			{
			int local_histogram[max_dist] = {0};
			float x, y, z;
			#pragma omp for
			for (size_t i = 0; i < nr_read_start; i+=dimension) {
				for (size_t j = 0; j < nr_read_end; j+=dimension) {
					x = start_points[i]-end_points[j];
					y = start_points[i+1]-end_points[j+1];
					z = start_points[i+2]-end_points[j+2];
					local_histogram[(int)(sqrtf(x*x+y*y+z*z)*100)] += 1;
				}
			}
			#pragma omp critical
			{
			for (size_t i = 0; i < max_dist; i++) {
				histogram[i] += local_histogram[i];}
			}}

		} while (nr_read_end == size*dimension);

		fseek(fp, block_end, SEEK_SET);
	} while (nr_read_start == size*dimension);

	for (size_t i = 0; i < max_dist; i++)
		if (histogram[i] != 0)
			printf("%02d.%02d %d\n", i/100, i%100, histogram[i]);
	
	fclose(fp);
	free(start_points);
	free(end_points);
	return 0;
}
