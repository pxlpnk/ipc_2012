#include <stdio.h>
#include <stdlib.h>

// MPI header
#include <mpi.h>
#include <getopt.h>
#include <errno.h>
#include "util.h"
#include "main.h"


#define root 0

typedef enum algo {ref, allgather} algo_t;


void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y) {
  for(uint i=0; i<n; i++) {
    y[i]=0;
    for(uint j=0; j<m; j++) {
      y[i] += x[i][j] * a[j];
    }
  }
}


bool test_vector_part(ATYPE *vector, ATYPE *reference, uint n, uint offset) {
  for(uint i = offset; i<n; i++) {
    if (vector[i] != reference[i])
      return false;
  }
  return true;
}





int main(int argc, char** argv) {
  int ret = EXIT_SUCCESS;
  int rank, size, N;
  char opt;
  int nt = -1;
  int max_threads = 16; // on jupiter

  bool id = false;
  algo_t algo = allgather;
  FILE *f = NULL;
	static const char optstring[] = "n:a:f:i:p:";
  static const struct option long_options[] = {
		{"n",			1, NULL, 'n'},
    {"file",		1, NULL, 'f'},
    {"i",			1, NULL, 'i'},
		{NULL,			0, NULL, 0},
  };


  MPI_Init(&argc,&argv);
  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);


	while ((opt = getopt_long(argc, argv, optstring, long_options, NULL)) != EOF) {
    switch(opt) {
    case 'i':
      if (strcmp("procs", optarg) == 0) {
        id = true;
      }
      break;
    case 'p':
      nt = atoi(optarg);
      if (nt > max_threads) {
        printf("Using too much procs %d, use max %d", nt, max_threads);
        return EXIT_FAILURE;
      } else {
        printf("Using %d procs.", nt);
      }
    case 'n':
      N = atoi(optarg);
      break;
    case 'f':
			f = fopen(optarg,"a");
			if (f == NULL) {
				mpi_printf(root, "Could not open log file '%s': %s\n", optarg, strerror(errno));
        MPI_Finalize();
				return  EXIT_FAILURE;
			}
			break;
    case 'a':
      if (strcmp("ref", optarg) == 0) {
        mpi_printf(root, "Using reference implementation \n");
        algo = ref;
      } else if ((strcmp("allgather", optarg) == 0)) {
        mpi_printf(root, "Using MPI_Allgather implementation \n");
        algo = allgather;
      }
      break;
    default:
      MPI_Finalize();
      return  EXIT_FAILURE;
    }
  }

  if(N == 0) {
		if ( rank == root ){
      printf("Usage: mpirun -nn nodecount p3-allgather.exe -n N\n");
      printf("N is the the matrix size. \n\n");
		}
    MPI_Finalize();
    return  EXIT_FAILURE;
  }


  ATYPE sendbuff[N], recvbuff[N], result[N];

  /* ======================================================== */
  /* Initialisation matrix & vector */

  debug("Setting up data structures");

  ATYPE **matrix = (ATYPE **) malloc(sizeof(ATYPE *) * N);
  if (matrix == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  for (uint i = 0; i < N; i++) {
    matrix[i] = (ATYPE *) malloc(sizeof(ATYPE) * N);
    if (matrix[i] == NULL) {
      fprintf(stderr, "Out of memory\n");
      return 1;
    }
  }
  matrix = fillMatrix(matrix, N, N);
  //  print_matrix(matrix, N, N);


  ATYPE *vector = NULL;
  vector = (ATYPE *) malloc(sizeof(ATYPE) * N);
  if (vector == NULL) {
    fprintf(stderr,"Out of memory\n");
    return 1;
  }

  vector = fillArr(vector, N);
  //  printArray(vector, N);

  ATYPE *reference = NULL;
  reference = (ATYPE *) malloc(sizeof(ATYPE) * N);
  if (reference == NULL) {
    fprintf(stderr,"Out of memory\n");
    return 1;
  }

  double  inittime = -1,totaltime = -1;

  if( algo == ref) {
    if (rank == root) {
      inittime = MPI_Wtime();
      matrix_vector_mult_ref(matrix, vector, N , N, reference);
      totaltime = MPI_Wtime() - inittime;
    }
  } else if ( algo == allgather) {

    debug("Comptuting reference");

    matrix_vector_mult_ref(matrix, vector, N , N, reference);


    /* ======================================================== */
    /* Initialisation send and receive buffers */

    debug("Setting up send and receive buffers");

    for(int i=0; i<N; i++) {
      recvbuff[i] = vector[i];
    }

    int partition = N/size;

    for(int i= (rank * partition); i < ((rank + 1 ) * partition) ; i++) {
      sendbuff[i] = 0;
      for (int k=0; k<N; k++) {
        sendbuff[i] = sendbuff[i] + matrix[i][k] * vector[k];
      }
    }

    debug("begin MPI_Allgather");

    inittime = MPI_Wtime();

    MPI_Allgather(&sendbuff, partition, ATYPE_MPI,
                  &recvbuff, partition, ATYPE_MPI,
                  MPI_COMM_WORLD);

    totaltime = MPI_Wtime() - inittime;
    double mytime = totaltime;

    MPI_Reduce(&mytime, &totaltime, 1, MPI_DOUBLE, MPI_MAX, root,  MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    debug("after MPI_Allgather");

    if (rank == root) {
      debug("Testing result");
      if (test_vector_part(result, recvbuff, (rank * partition) , partition)) {
        debug("testresult: OK");
      } else {
        debug("testresult: FAILURE");
        debug("Result:");
        printArray(recvbuff, N);
        debug("Reference:");
        printArray(reference,N);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  if (rank == 0) {
    if (f != NULL) {
      if (id) {
        fprintf(f,"%d,%lf\n",nt, totaltime);
      } else {
        fprintf(f,"%d,%lf\n",N, totaltime);
      }
    }
    if (id) {
      printf("%d,%lf\n",nt , totaltime);
    } else {
      printf("%d,%lf\n",N , totaltime);
    }
  }


  debug("cleaning up");

  free(vector);
  for(int i=0; i<N; i++) {
    free(matrix[i]);
  }

  free(matrix);

  MPI_Finalize();
	if (f != NULL)
		fclose(f);
	return ret;
}
