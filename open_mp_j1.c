#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

int main (int argc, char *argv[]) {
	int size = 40;
	int dimension = 3;
	int max_dist = 3465;
	FILE *fp = fopen("data.txt", "r");
	int *histogram = (int*) malloc(sizeof(int)*max_dist);
	short int *points = (short int*) malloc(sizeof(short int)*dimension*size); // <5MiB
	//float **points = (float**) malloc(sizeof(float*)*size);
	//for (size_t i = 0, j = 0; i < size; i++, j+=dimension)
	//	points[i] = cords + j;
	int length = -1;
	float dist;
	omp_set_num_threads(8);
	clock_t time = clock();

	// Read as char
	// Read in parallel
	// double?

	short int number;
	char array[6];
	for (size_t i = 0; i < size*dimension; i+=dimension) {
		if (fread(array, 1, 3, fp) == 3) {
			fseek(fp, 1, SEEK_CUR);
			fread(array+3, 1, 3, fp);
			fseek(fp, 1, SEEK_CUR);
			number = (array[1]-'0')*10000+(array[2]-'0')*1000+(array[3]-'0')*100
							 +(array[4]-'0')*10+array[5]-'0';
			if (array[0] == '-')
				number *= -1;
			points[i] = number;
			fread(array, 1, 3, fp);
			fseek(fp, 1, SEEK_CUR);
			fread(array+3, 1, 3, fp);
			fseek(fp, 1, SEEK_CUR);
			number = (array[1]-'0')*10000+(array[2]-'0')*1000+(array[3]-'0')*100
							 +(array[4]-'0')*10+array[5]-'0';
			if (array[0] == '-')
				number *= -1;
			points[i+1] = number;
			fread(array, 1, 3, fp);
			fseek(fp, 1, SEEK_CUR);
			fread(array+3, 1, 3, fp);
			fseek(fp, 1, SEEK_CUR);
			number = (array[1]-'0')*10000+(array[2]-'0')*1000+(array[3]-'0')*100
							 +(array[4]-'0')*10+array[5]-'0';
			if (array[0] == '-')
				number *= -1;
			points[i+2] = number;
		} else {
			length = i/3;
			break;
		}
	}

	for (size_t i = 0; i < length; i++)
		printf("%f, %f, %f\n", (float)points[i]/1000, (float)points[i+1]/1000, 
									  (float)points[i+2]/1000);
	

	/*while (length == -1) {
		for (size_t i = 0; i < size*dimension; i+=dimension) {
			if (fscanf(fp, "%f", &points[i]) == 1) {
				// not for loop for efficiency
				fscanf(fp, "%f", &points[i+1]);
				fscanf(fp, "%f", &points[i+2]);
			} else {
				printf("Hello\n");
				length = i;
				break;
			}
		}
		
		#pragma omp parallel for
		for (size_t i = 0; i < length; i+=3) {
			for (size_t j = i+3; j < length; j+=3) {
				dist = sqrtf((points[i]-points[j])*(points[i]-points[j])+
								 (points[i+1]-points[j+1])*(points[i+1]-points[j+1])+
								 (points[i+2]-points[j+2])*(points[i+2]-points[j+2]));
				histogram[(int)(dist*100)] += 1; // collisions highly unlikely
			}
		}
	}

	double avg_time = (double)(clock() - time) / CLOCKS_PER_SEC;

	for (size_t i = 0; i <= max_dist; i++)
		if (histogram[i] != 0)
			printf("%02d.%02d %d\n", i/100, i%100, histogram[i]);
	printf("Runtime: %f\n", avg_time);
	*/
	fclose(fp);
	free(histogram);
	free(points);
	return 0;
}
