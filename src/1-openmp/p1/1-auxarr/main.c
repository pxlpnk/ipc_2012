#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../../../shared/util.h"

bool Scan(ATYPE *arr, uint n, uint *opsp)
{
	if (n == 1)
		return true;

	ATYPE *tmp = malloc(sizeof(ATYPE) * n);
	if (tmp == NULL)
		return false;

	bool ret = true;
	uint ops = *opsp;
	#pragma omp parallel reduction(&&: ret), reduction(+: ops)
	{
		unsigned int i;
		#pragma omp for
		for (i = 0; i < n/2; i++) {
			tmp[i] = arr[2*i] + arr[2*i + 1];
			ops++;
		}
		// implicit barrier

		#pragma omp single
		{
			ret = Scan(tmp, n/2, &ops);
			arr[1] = tmp[0];
		} // implicit barrier

		#pragma omp for nowait
		for (i = 1; i < n/2; i++) {
			arr[2*i] = tmp[i-1] + arr[2*i];
			ops++;
			arr[2*i + 1] = tmp[i];
		}
		#pragma omp single nowait
		{
		if (odd(n))
			arr[n-1] = tmp[n/2-1] + arr[n-1];
			ops++;
		}
	} // implicit barrier

	*opsp = ops;
	return ret;
}

bool printArrs(ATYPE *a, ATYPE *b, uint n) {
	for (uint i = 0; i < n; i++) {
		printf("[%03d]:\t%" ATYPEPRINT, i, a[i]);
		printf("\t%s    ", (a[i] != b[i]) ? "!=    " : "      ");
		printf("%" ATYPEPRINT "\n", b[i]);
	}
	printf("\n");
	return false;
}

int main (int argc, char *argv[]) {
	uint nt = (argv[1] != NULL) ? atoi(argv[1]) : 0; // TODO: error handling
	if (nt > 0)
		omp_set_num_threads(nt);

	#pragma omp parallel
	{
		#pragma omp  single
		nt = omp_get_num_threads();
	}

	//uint n = 1<<20; // 1MB
	uint n = 5;
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
	bool ret = Scan(arr, n, &ops);
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

