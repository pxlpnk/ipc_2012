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

#ifndef DBG
#undef printf
#define printf(...)
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

/* calculates and returns the sum of elements in arr and puts the prefix sums in sums. */
uint prefixSums(ATYPE *arr, uint n, bool exclusive, ATYPE * sums);

/* print the array with indices */
void printArray(ATYPE *a, uint n);

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
