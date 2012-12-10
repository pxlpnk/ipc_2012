#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef unsigned int uint;
#define odd(x) (((x) % 2) == 1)

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
