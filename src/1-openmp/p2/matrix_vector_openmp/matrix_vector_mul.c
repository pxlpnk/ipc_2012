#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "matrix_vector_mul.h"


void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y) {
  for(uint i=0; i<n; i++) {
    y[i]=0;
    for(uint j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
    }
  }
}


void matrix_vector_mult_tiling(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y) {
  #pragma omp parallel
  {
    uint rank = omp_get_thread_num();
    uint p = omp_get_num_threads();

    for(uint i=rank*n/p; i< (rank + 1)*n/p; i++) {
      y[i] = 0;
      for(uint j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
      }
    }
  }
}

void matrix_vector_mult_false_sharing(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y) {
  #pragma omp parallel
  {
    uint rank = omp_get_thread_num();
    uint p = omp_get_num_threads();
    for(uint i=rank; i<n; i+=p) {
      y[i]=0;
      for(uint j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
      }
    }
  }
}
