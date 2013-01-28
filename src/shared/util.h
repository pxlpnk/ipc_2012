#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#ifndef __CILK__
	#include <stdbool.h>
#else
	#define bool int
	#define true 1
	#define false 0
#endif

#ifdef DBG
#define mpi_printf(r, ...) \
	if (rank == r)\
		printf(__VA_ARGS__)
#else
#undef printf
#define printf(...) /* compiled away */
#define mpi_printf(...) /* compiled away */
#endif

typedef unsigned int uint;
#define odd(x) (((x) % 2) == 1)

uint roundUpPowerOf2(uint v);

/* returns random value v \elem [min, max]. see http://c-faq.com/lib/randrange.html */
uint randRange(uint min, uint max);

/* shuffles the elements in array arr of size n */
void shuffleArr(ATYPE *arr, uint n);

/* fills array arr of size n with a counter starting at 0 */
ATYPE *fillArr(ATYPE *arr, uint n);

/* calculates the exclusive prefix sums of (and *in*) the elements in arr up to and including offset n */
uint prefixSumEx(ATYPE *arr, uint n);

/* calculates and returns the sum of elements in arr and puts the prefix sums in sums. */
uint prefixSums(ATYPE *arr, uint n, bool exclusive, ATYPE * sums);

/* print the array with indices */
void printArray(ATYPE *a, uint n);

/* compares and prints a single element of two arrays */
void printArrsElem(ATYPE *a, ATYPE *b, uint i);

/* print two arrays side by side with indices and show differences in elements */
void printArrs(ATYPE *a, ATYPE *b, uint n);

/* prints a matrix */
void print_matrix(ATYPE **matrix, uint m, uint n);

/* creates a nxm matrix, fills it with predefined data and returns it */
ATYPE **fillMatrix(ATYPE **matrix, uint m, uint n);

/* tests if two vectors are equal */
bool testResult(ATYPE *vector, ATYPE *reference, uint n);

/* returns the minimum of two integers */
int min(uint x, uint y);

/*returns the current time as time stamp */
char *time_stamp();

/* prints debug messages when debug is set */
void debug (char *msg);

/* just get me out of here */
void usage_abort(void);
