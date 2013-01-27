#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>




#include "matrix_vector_mul.h"

typedef enum algo {ref, false_sharing, tiling} algo_t;


double compute_ref(ATYPE **matrix, ATYPE *vector,uint n, uint m, ATYPE *ref_output) {

  double time = omp_get_wtime();
  matrix_vector_mult_ref(matrix, vector, n, m, ref_output);
	time = omp_get_wtime() - time;

  return time;
}

double compute_false_sharing(ATYPE **matrix, ATYPE *vector,uint n, uint m, ATYPE *output) {

  double time = omp_get_wtime();
  matrix_vector_mult_false_sharing(matrix, vector, n, m, output);
  time = omp_get_wtime() - time;

  return time;
}

double compute_tiling_outer_loop(ATYPE **matrix, ATYPE *vector,uint n, uint m, ATYPE *output) {

  double time = omp_get_wtime();
  matrix_vector_mult(matrix, vector, n, m, output);
  time = omp_get_wtime() - time;

  return time;
}



int main (int argc, char *argv[]) {
  uint n = -1;
  uint m = -1;
  uint max_threads = omp_get_max_threads();
  uint nt = max_threads;
  double mtime = 0.0;
  char opt;

  algo_t algo = tiling;
  FILE *f = NULL;
	static const char optstring[] = "p:n:m:a:f:";
  static const struct option long_options[] = {
		{"n",			1, NULL, 'n'},
    {"file",		1, NULL, 'f'},
		{NULL,			0, NULL, 0},
  };

	while ((opt = getopt_long(argc, argv, optstring, long_options, NULL)) != EOF) {
    switch(opt) {
    case 'p':
      nt = atoi(optarg);
      if (nt > max_threads) {
        printf("Using too much procs %d, use max %d", nt, max_threads);
        return EXIT_FAILURE;
      } else {
        printf("Using %d procs.", nt);
      }
      break;
    case 'n':
      n = atoi(optarg);
      break;
    case 'm':
      m = atoi(optarg);
      break;
    case 'f':
			f = fopen(optarg,"a");
			if (f == NULL) {
				printf("Could not open log file '%s': %s\n", optarg, strerror(errno));
				return  EXIT_FAILURE;
			}
			break;
    case 'a':
      if (strcmp("ref", optarg) == 0) {
        printf("Using reference implementation\n");
        algo = ref;
      } else if ((strcmp("false_sharing", optarg) == 0)) {
        printf( "Using false_sharing implementation\n");
        algo = false_sharing;
      } else if ((strcmp("tiling", optarg) == 0)) {
        printf("Using false_sharing implementation \n");
        algo = tiling;
      }
      break;
    default:
      return  EXIT_FAILURE;
    }
  }

  if( n == -1 && m != -1) {
    printf("No n given using m\n");
    n = m;
  } else if (n != -1 && m == -1) {
    printf("No m given using n\n");
    m = n;
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


  if (algo == ref ){
    printf("reference:\n");
    mtime = compute_ref(matrix, vector, n, m, ref_output);
    printf("%lf seconds.\n",mtime);
  } else if (algo == false_sharing) {
    compute_ref(matrix, vector, n, m, ref_output);
    printf("No of cpus: %d\n", nt);
    omp_set_num_threads(nt);
    mtime = compute_false_sharing(matrix, vector, n, m, product);
  } else if (algo == tiling) {
    compute_ref(matrix, vector, n, m, ref_output);
    printf("No of cpus: %d\n", nt);
    omp_set_num_threads(nt);
    mtime =  compute_tiling_outer_loop(matrix, vector, n, m, product);
  }


  if (f != NULL) {
    fprintf(f,"%dx%d,%lf\n",n,m, mtime);
  }

  printf("%dx%d,%lf\n",n,m , mtime);


#ifdef DBG
  if ( !testResult(product, ref_output, n) && algo != ref ) {
    printArray(product, n);
    printArray(ref_output,n);
    printf("=======> Wrong result\n");
    return 1;
  }
#endif



  // cleaning up
  free(vector);
  for(int i=0; i<n; i++) {
    free(matrix[i]);
  }

  free(matrix);
  free(ref_output);
  free(product);
  fclose(f);

}
