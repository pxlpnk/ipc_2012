#include <stdio.h>
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

void printArray(ATYPE *a, uint n) {
	for (uint i = 0; i < n; i++) {
		printf("[%03d]:\t%" ATYPEPRINT "\n", i, a[i]);
	}
	printf("\n");
}

void printArrs(ATYPE *a, ATYPE *b, uint n) {
	for (uint i = 0; i < n; i++) {
		printf("[%03d]:\t%" ATYPEPRINT, i, a[i]);
		printf("\t%s    ", (a[i] != b[i]) ? "!=    " : "      ");
		printf("%" ATYPEPRINT "\n", b[i]);
	}
	printf("\n");
}


void print_matrix(ATYPE **matrix, uint m, uint n) {
  printf("#### MATRIX\n");
  for (uint i = 0; i < n; i++) {
    printf("[%d] ", i);
    for (uint j = 0; j < m; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }
}

ATYPE **fillMatrix(ATYPE **matrix, uint n, uint m) {
  for (uint i=0; i<n; i++) {
    for (uint j=0; j<m; j++) {
      matrix[i][j] = ((j*i) % 42) + m;
    }
  }
  return matrix;
}


bool testResult(ATYPE *vector, ATYPE *reference, uint n) {
  for(uint i = 1; i<n; i++) {
    if (vector[i] != reference[i])
      return false;
  }
  return true;
}


int min(uint x, uint y) {
    return (x < y) ? x : y;
}


char *time_stamp() {
  time_t now = time(NULL);
  struct tm tim;
  tim = *(localtime(&now));
  static char timestamp[13];
  strftime(timestamp, 14, "%Y%m%d-%H%M", &tim);
  return timestamp;
}
