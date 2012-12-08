#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../../../shared/util.h"

// FIXME: still buggy :P
bool Scan(ATYPE *arr, unsigned int n)
{
	if (n == 1)
		return true;

	ATYPE *tmp = malloc(sizeof(ATYPE) * n);
	if (tmp == NULL)
		return false;

	bool ret = true;
	#pragma omp parallel reduction(&&: ret)
	{
		unsigned int i;
		#pragma omp parallel for
		for (i = 0; i < n/2; i++)
			tmp[i] = arr[2*i] + arr[2*i + 1];
		// implicit barrier

		if (!Scan(tmp, n/2))
			ret = false;
		#pragma omp barrier

		arr[1] = tmp[0];
		#pragma omp for nowait
		for (i = 1; i < n/2; i++) {
			arr[2*i] = tmp[i-1] + arr[2*i];
			arr[2*i + 1] = tmp[i];
		}
		#pragma omp single nowait
		if (odd(n))
			arr[n-1] = tmp[n/2-1]+arr[n-1];
	}

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
	double time = omp_get_wtime();

	#pragma omp parallel
	{
		#pragma omp  single
		printf("There are %d threads\n",omp_get_num_threads());
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

	Scan(arr, n);
	if (n < 100)
		printArrs(cor, arr, n);

	printf("Correct? %s\n", memcmp(cor, arr, sizeof(ATYPE) * n) == 0 ? "yes" : "no");

	free(cor);
	free(arr);

	time = omp_get_wtime() - time;
	printf("%lf seconds.\n",time);
	return EXIT_SUCCESS;
}

