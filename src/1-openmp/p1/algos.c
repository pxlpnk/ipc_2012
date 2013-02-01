#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <omp.h>
#include "util.h"

void seq(ATYPE *x, uint n, uint *opsp) {
	uint ops = *opsp;
	uint sum = x[0];
	x[0] = 0; // undefined
	for (uint i = 1; i < n; i++) {
		sum += x[i];
		x[i] = sum;
		ops++;
	}
	*opsp = ops;
}

void reduction(ATYPE *x, uint n, uint *opsp) {
	uint ops = *opsp;
	uint sum = x[0];
	#pragma omp parallel for ordered reduction(+:sum) reduction(+: ops)
	for (uint i = 1; i < n; i++) {
		x[i] = sum;
		sum += x[i];
		ops++;
	}
	*opsp = ops;
}

void inplace(ATYPE *x, uint n, uint *opsp) {
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
}

void auxarr(ATYPE *x, uint n, uint *opsp) {
	if (n == 1)
		return;

	ATYPE *tmp = malloc(sizeof(ATYPE) * n/2);
	if (tmp == NULL) {
		fprintf(stderr, "Out of memory, result undefined.\n");
		exit(1);
	}

	uint ops = *opsp;
	#pragma omp parallel reduction(+: ops)
	{
		unsigned int i;
		#pragma omp for
		for (i = 0; i < n/2; i++) {
			tmp[i] = x[2*i] + x[2*i + 1];
			ops++;
		} // implicit barrier

		#pragma omp single
		{
			auxarr(tmp, n/2, &ops);
			x[1] = tmp[0];
		} // implicit barrier

		#pragma omp for nowait
		for (i = 1; i < n/2; i++) {
			x[2*i] = tmp[i-1] + x[2*i];
			ops++;
			x[2*i + 1] = tmp[i];
		}
		#pragma omp single nowait
		{
		if (odd(n))
			x[n-1] = tmp[n/2-1] + x[n-1];
			ops++;
		}
	} // implicit barrier

	free(tmp);
	*opsp = ops;
}

void hillis(ATYPE **arr, uint n, uint *opsp) {
	uint ops = *opsp;

	ATYPE *y = malloc(n*sizeof(ATYPE));
	ATYPE *x = *arr;
	ATYPE *t;
	for (uint k = 1; k < n; k <<= 1) {
		#pragma omp parallel reduction(+: ops)
		{
			#pragma omp for nowait
			for (uint i = 0; i < k; i++) {
				y[i] = x[i];
				ops++;
			}
			#pragma omp for
			for (uint i = k; i < n; i++) {
				y[i] = x[i-k]+x[i];
				ops++;
			} // implicit barrier
		}
		t = x; x = y; y = t; // swap
	}

	free(y);
	*opsp = ops;
	*arr = x;
}
