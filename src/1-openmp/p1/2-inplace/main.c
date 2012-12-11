#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../../../shared/util.h"

bool inplace(ATYPE *x, uint n, uint *opsp) {
	bool ret = true;
	uint ops = *opsp;
	uint k, kk;
	for (k = 1; k < n; k = kk) {
		kk = k<<1; // double
		#pragma omp parallel reduction(+: ops)
		{
			#pragma omp for
			for (uint i = kk-1; i < n; i += kk) {
				x[i] = x[i-k] + x[i];
				ops++;
			} // implicit barrier
		}
	}

	for (k = k>>1; k > 1; k = kk) {
		kk = k>>1; // halve
		#pragma omp parallel reduction(+: ops)
		{
			#pragma omp for
			for (uint i = k-1; i < n-kk; i += k) {
				x[i+kk] = x[i] + x[i+kk];
			} // implicit barrier
		}
	}

	*opsp = ops;
	return ret;
}

int main (int argc, char *argv[]) {
	uint nt = (argv[1] != NULL) ? atoi(argv[1]) : 0; // TODO: error handling
	if (nt > 0)
		omp_set_num_threads(nt);

	uint n = (argv[2] != NULL) ? atoi(argv[2]) : 0; // TODO: error handling
	if (n == 0) {
		printf("N not given!\n");
		return EXIT_FAILURE;
	}

	#pragma omp parallel
	{
		#pragma omp  single
		nt = omp_get_num_threads();
	}

	ATYPE *arr = malloc(sizeof(ATYPE) * n);
	if (arr == NULL)
		return EXIT_FAILURE;

	arr = fillArr(arr, n);

	ATYPE *cor = malloc(sizeof(ATYPE) * n);
	if (cor == NULL)
		return EXIT_FAILURE;

	prefixSums(arr, n, false, cor);

	uint ops = 0;
	double time = omp_get_wtime();
	bool ret = inplace(arr, n, &ops);
	time = omp_get_wtime() - time;
	if (!ret) {
		printf("Scan returned an error!\n");
		goto out;
	}
	printf("%d additions of array elements executed in %lf seconds by %d threads\n", ops, time, nt);

	if (n < 100)
		printArrs(cor, arr, n);

	printf("Correct? %s\n", memcmp(cor, arr, sizeof(ATYPE) * n) == 0 ? "yes" : "no");

out:
	free(cor);
	free(arr);

	return ret ? EXIT_SUCCESS : EXIT_FAILURE;
}

