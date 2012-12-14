#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "../../../shared/util.h"

void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y) {
  for(int i=0; i<n; i++) {
    y[i]=0;
    for(int j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
    }
  }
}


void matrix_vector_mult(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y) {
  #pragma omp parallel
  {
    int rank = omp_get_thread_num();
    int p = omp_get_num_threads();

    for(int i=rank*n/p; i< (rank + 1)*n/p; i++) {
      y[i] = 0;
      for(int j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
      }
    }
  }
}

void matrix_vector_mult_false_sharing(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y) {
  #pragma omp parallel
  {
    int rank = omp_get_thread_num();
    int p = omp_get_num_threads();
    for(int i=rank; i<n; i+=p) {
      y[i]=0;
      for(int j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
      }
    }
  }
}



int main (int argc, char *argv[]) {
  int n = 0;
  int m = 0;

  if (argc != 3) {
    n = 1000;
    m = 100;
  } else {
    n = atoi(argv[1]);
    m = atoi(argv[2]);
  }

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
  double ref_time = omp_get_wtime();
  matrix_vector_mult_ref(matrix, vector, n, m, ref_output);
	ref_time = omp_get_wtime() - ref_time;

  printf("%lf seconds.\n",ref_time);


  printf("tiling outer loop(false_sharing):\n");
  double time = omp_get_wtime();
  matrix_vector_mult_false_sharing(matrix, vector, n, m, product);
  time = omp_get_wtime() - time;
  printf("%lf seconds.\n",time);

  if ( !testResult(product, ref_output, n)) {
    printf("=======> Wrong result\n");
    return 1;
  }

  free(product);
  product = (ATYPE *) malloc(sizeof(ATYPE) *n);
  if(product == NULL) {
    fprintf(stderr,"Out of memory\n");
    return 1;
  }


  printf("tiling outer loop propper:\n");
  time = omp_get_wtime();
  matrix_vector_mult(matrix, vector, n, m, product);
  time = omp_get_wtime() - time;
  printf("%lf seconds.\n",time);

  if ( !testResult(product, ref_output, n)) {
    printf("=======> Wrong result\n");
    return 1;
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
