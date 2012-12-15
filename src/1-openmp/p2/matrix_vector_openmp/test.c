#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "matrix_vector_mul.h"


uint compute_ref(ATYPE **matrix, ATYPE *vector,uint n, uint m, ATYPE *ref_output) {

  double time = omp_get_wtime();
  matrix_vector_mult_ref(matrix, vector, n, m, ref_output);
	time = omp_get_wtime() - time;

  printf("%lf seconds.\n", time);
  return time;
}

uint compute_false_sharing(ATYPE **matrix, ATYPE *vector,uint n, uint m, ATYPE *output) {

  double time = omp_get_wtime();
  matrix_vector_mult_false_sharing(matrix, vector, n, m, output);
  time = omp_get_wtime() - time;

  printf("%lf seconds.\n",time);
  return time;
}

uint compute_tiling_outer_loop(ATYPE **matrix, ATYPE *vector,uint n, uint m, ATYPE *output) {

  double time = omp_get_wtime();
  matrix_vector_mult(matrix, vector, n, m, output);
  time = omp_get_wtime() - time;

  printf("%lf seconds.\n",time);
  return time;
}



int main (int argc, char *argv[]) {
  uint n = 400;
  uint m = 4000;
  uint nt = 3;
  uint max_threads = omp_get_max_threads();

  printf("setting up data structures\n");

  // preparing data structures
  ATYPE *vector = NULL;
  ATYPE *product = NULL;
  ATYPE *ref_output = NULL;

  // matrix

  ATYPE **matrix = (ATYPE **) malloc(sizeof(ATYPE *) * n);
  if (matrix == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  for (uint i = 0; i < n; i++) {
    matrix[i] = (ATYPE *) malloc(sizeof(ATYPE) * m);
    if (matrix[i] == NULL) {
      fprintf(stderr, "Out of memory\n");
      return 1;
    }
  }


  matrix = fillMatrix(matrix, n, m);

  // vector
  vector = (ATYPE *) malloc(sizeof(ATYPE) * m);
  if (vector == NULL) {
    fprintf(stderr,"Out of memory\n");
    return 1;
  }

  // resulting product
  product = (ATYPE *) malloc(sizeof(ATYPE) *n);
  if(product == NULL) {
    fprintf(stderr,"Out of memory\n");
    return 1;
  }

  // reference output
  ref_output = (ATYPE *) malloc(sizeof(ATYPE) * n);
  if (ref_output == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  // add some data to our matrix

  vector = fillArr(vector, m);

  for(int i=0; i<m; i++) {
    vector[i]=i+m;
  }

  printf("Starting computation:\n");

  printf("reference:\n");
  compute_ref(matrix, vector, n, m, ref_output);

  for(uint i = 1; i<= nt; i++) {

    omp_set_num_threads(i);

    printf("tiling outer loop(false_sharing):\n");
    compute_false_sharing(matrix, vector, n, m, product);

#ifdef DEBUG
    if ( !testResult(product, ref_output, n)) {
      printf("=======> Wrong result\n");
      return 1;
    }
#endif

    free(product);
    product = (ATYPE *) malloc(sizeof(ATYPE) *n);
    if(product == NULL) {
      fprintf(stderr,"Out of memory\n");
      return 1;
    }

    printf("tiling outer loop propper:\n");
    compute_tiling_outer_loop(matrix, vector, n, m, product);

#ifdef DEBUG
    printf("fffoo");
    if ( !testResult(product, ref_output, n)) {
      printf("=======> Wrong result\n");
      return 1;
    }
#endif

  }
  // cleaning up
  free(vector);
  for(int i=0; i<n; i++) {
    free(matrix[i]);
  }

  free(matrix);
  free(ref_output);
  free(product);

}
