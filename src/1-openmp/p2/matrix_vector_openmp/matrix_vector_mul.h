#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y);

void matrix_vector_mult(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y);

void matrix_vector_mult_false_sharing(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y);
