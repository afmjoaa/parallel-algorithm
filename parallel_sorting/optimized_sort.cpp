#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare(const void* numA, const void* numB);
double * getTimesForReadSortAndPrint(char *argv[]);

int main(int argc, char *argv[]) {

	if (argc < 4) {
		printf("You have not supplied enough command line parameters\n");
		printf("Usage: ./program-name ${input-file-path} ${number-of-integers-to-sort} ${number_of_iteration_for_avaraging}\n");
		exit(-1);
	}

	int number_of_iteration = atoi(argv[3]);
	double summation_of_times[3];
	
	for (int i = 0; i < number_of_iteration; i++) {
		double *times;
   		times = getTimesForReadSortAndPrint(argv);
		for(int j = 0; j < 3; j++) {
			summation_of_times[j] += times[j];
		}
	}

	printf("\n\nAverage time taken to read the input for %d iterations is: %f seconds\n", number_of_iteration, (summation_of_times[0]/number_of_iteration));
	printf("\n\nAverage time taken to sort the array for %d iterations is: %f seconds\n", number_of_iteration, (summation_of_times[1]/number_of_iteration));
	printf("\n\nAverage time taken to print the sorted array for %d iterations is: %f seconds\n", number_of_iteration, (summation_of_times[2]/number_of_iteration));

	return 0;
}


int compare(const void* numA, const void* numB) {
    
	const int* num1 = (const int*)numA;
	const int* num2 = (const int*)numB;

    if (*num1 > *num2) {
		return 1;
   	}
    else {
        if (*num1 == *num2) {
            return 0;
		}
        else {
            return -1;
		}
    }
}


double * getTimesForReadSortAndPrint(char *argv[]) {

	static double times[3];

	clock_t start_time;
	start_time = clock();

	FILE *fp;
	if ((fp = fopen(argv[1], "r+")) == NULL) {
			printf("Could not open the file. probably the file does not exist.\n");
			exit(-1);
		}

	int count = atoi(argv[2]);
	int *array = (int *) malloc(count * sizeof(int));

	for (int i = 0; i < count; i++) {
		int status = fscanf(fp, "%d", &array[i]);
		if (status != 1) {
			printf("Some error happened when reading numbers from file. Only read %d items.\n", i);
			exit(-1);
		}	
	}

	clock_t end_time;
	end_time = clock();
	double time_taken =  ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
	times[0] = time_taken;


	start_time = clock();
	qsort(array, count, sizeof(int), compare);
	end_time = clock();
	time_taken =  ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
	times[1] = time_taken;


	start_time = clock();
	printf("The sorted array is:\n");
	for (int i = 0; i < count; i++) {
		printf("%d ", array[i]);
	} 
	printf("\n");
	end_time = clock();
	time_taken =  ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
	times[2] = time_taken;


	fclose(fp);
	free(array);

	return times;
}
