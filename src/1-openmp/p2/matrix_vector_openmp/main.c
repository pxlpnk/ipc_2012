#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void print_vector(long *v, int m);
void print_matrix(long **matrix, int m, int n);

void matrix_vector_mult_ref(long **x, long *a, int n, int m, long *y) {
  for(int i=0; i<n; i++) {
    y[i]=0;
    for(int j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
    }
  }
}


void print_vector(long *v, int m) {
  printf("#### VECTOR\n");
  for (int i=0; i<m; i++) {
    printf("[%d] %ld\n", i, v[i]);
  }
}

void print_matrix(long **matrix, int m, int n) {
  printf("#### MATRIX\n");
  for (int i = 0; i < n; i++) {
    printf("[%d] ", i);
    for (int j = 0; j < m; j++) {
      printf("%ld ", matrix[i][j]);
    }
    printf("\n");
  }
}

int main (int argc, char *argv[]) {

  // preparing data structures

  // matrix
  int n = 1000000;
  int m = 1000000;
  long ** matrix;
  long *vector;
  long *product;

  matrix = (long **) malloc(sizeof(long *) * n);
  if (matrix == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  for (int i=0; i<n; i++) {
    matrix[i] = (long *) malloc(sizeof(long) * m);
    if (matrix[i] == NULL) {
      fprintf(stderr,"Out of memory\n");
      return 1;
    }
  }

  // vector
  vector = (long *) malloc(sizeof(long) * m);
  if (vector == NULL) {
      fprintf(stderr,"Out of memory\n");
      return 1;
  }

  // resulting product
  product = (long *) malloc(sizeof(long) *n);
  if(product == NULL) {
    fprintf(stderr,"Out of memory\n");
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

  matrix_vector_mult_ref(matrix, vector, n, m, product);
  /* print_vector(vector,m); */
  /* print_matrix(matrix, m, n); */
  /* print_vector(product, m); */



  // cleaning up
  free(vector);
  for(int i=0; i<n; i++) {
    free(matrix[i]);
  }

  free(matrix);

}
