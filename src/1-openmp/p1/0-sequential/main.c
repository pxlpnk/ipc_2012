#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../../../shared/util.h"

int main (int argc, char *argv[]) {
	uint nt = (argv[1] != NULL) ? atoi(argv[1]) : 0; // TODO: error handling
	if (nt > 1) {
		printf("this program does not support more than 1 thread!\n");
		return EXIT_FAILURE;
	}
	omp_set_num_threads(1);

	uint n = (argv[2] != NULL) ? atoi(argv[2]) : 0; // TODO: error handling
	if (n == 0) {
		printf("N not given!\n");
		return EXIT_FAILURE;
	}

	ATYPE *arr = malloc(sizeof(ATYPE) * n);
	if (arr == NULL)
		return EXIT_FAILURE;

	arr = fillArr(arr, n);

	ATYPE *dst = malloc(sizeof(ATYPE) * n);
	if (dst == NULL)
		return EXIT_FAILURE;

	uint ops = 0;
	double time = omp_get_wtime();
	uint sum = arr[0];
	dst[0] = 0; // undefined
	for (uint i = 1; i < n; i++) {
		sum += arr[i];
		dst[i] = sum;
		ops++;
	}
	time = omp_get_wtime() - time;
	printf("%d additions of array elements executed in %lf seconds by 1 thread\n", ops, time);

	if (n < 100)
		printArray(dst, n);

	free(dst);
	free(arr);

	return EXIT_SUCCESS;
}

