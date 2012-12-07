#include "util.h"

uint randRange(uint min, uint max) {
	return min + (uint)((double)rand() / ((double)RAND_MAX / (max - min + 1) + 1));
}

void shuffleArr(ATYPE *arr, uint n) {
	ATYPE tmp;
	for (uint i = 0; i < n; i++) {
		tmp = arr[i];
		uint j = randRange(0, n-1);
		arr[i] = arr[j];
		arr[j] = tmp;
	}
}

ATYPE *fillArr(ATYPE *arr, uint n) {
	for (uint i = 0; i < n; i++)
		arr[i] = i;
	return arr;
}

uint prefixSums(ATYPE *arr, uint n, bool exclusive, ATYPE * sums) {
	uint sum = arr[0];
	sums[0] = 0; // undefined
	for (uint i = 1; i < n; i++) {
		if (exclusive) {
			sums[i] = sum;
			sum += arr[i];
		} else {
			sum += arr[i];
			sums[i] = sum;
		}
	}
	return sum;
}
