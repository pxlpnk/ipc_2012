#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "../../../shared/util.h"


void print_vector(ATYPE *v, int m);
void print_matrix(ATYPE **matrix, int m, int n);

void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, int n, int m, ATYPE *y) {
  for(int i=0; i<n; i++) {
    y[i]=0;
    for(int j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
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



void print_vector(ATYPE *v, int m) {
  printf("#### VECTOR\n");
  for (int i=0; i<m; i++) {
    printf("[%d] %ld\n", i, v[i]);
  }
}


void print_matrix(ATYPE **matrix, int m, int n) {
  printf("#### MATRIX\n");
  for (int i = 0; i < n; i++) {
    printf("[%d] ", i);
    for (int j = 0; j < m; j++) {
      printf("%ld ", matrix[i][j]);
    }
    printf("\n");
  }
}


ATYPE sum_vector(ATYPE *v, int n) {
  int aggregate = 0;
  for(int i=0; i<n; i++) {
    aggregate += v[i];
  }
  return aggregate;
}


int main (int argc, char *argv[]) {

  printf("setting up data structures\n");
  // preparing data structures

  // matrix
  int n = 100000;
  int m = 10000;
  ATYPE ** matrix;
  ATYPE *vector;
  ATYPE *product;
  ATYPE *ref_output;

  matrix = (ATYPE **) malloc(sizeof(ATYPE *) * n);
  if (matrix == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  for (int i=0; i<n; i++) {
    matrix[i] = (ATYPE *) malloc(sizeof(ATYPE) * m);
    if (matrix[i] == NULL) {
      fprintf(stderr,"Out of memory\n");
      return 1;
    }
  }

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
  ref_output = (long *) malloc(sizeof(long) * n);
  if (ref_output == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  // add some data to our matrix

  for (int i=0; i<n; i++) {
    for (int j=0; j<m; j++) {
      matrix[i][j] = ((j*i) % 42) + m;
    }
  }


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


  /* print_vector(vector,m); */
  /* print_matrix(matrix, m, n); */
  /* print_vector(product, m); */


  // cleaning up
  free(vector);
  for(int i=0; i<n; i++) {
    free(matrix[i]);
  }

  free(matrix);
  free(ref_output);
  free(product);

}
