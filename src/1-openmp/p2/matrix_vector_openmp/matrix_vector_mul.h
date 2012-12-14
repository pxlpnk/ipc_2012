#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "../../../shared/util.h"


void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y);

void matrix_vector_mult(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y);

void matrix_vector_mult_false_sharing(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y);
