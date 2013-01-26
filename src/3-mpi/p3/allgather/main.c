#include <stdio.h>
#include <stdlib.h>

// MPI header
#include <mpi.h>
#include <getopt.h>

#include "util.h"
#include "main.h"


#define root 0

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
  int rank, size, N;
  char opt;

	static const char optstring[] = "n:a:f:";
  static const struct option long_options[] = {
		{"n",			1, NULL, 'n'},
		{NULL,			0, NULL, 0}
  };


  MPI_Init(&argc,&argv);

  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	while ((opt = getopt_long(argc, argv, optstring, long_options, NULL)) != EOF) {
    switch(opt) {
    case 'n':
      N = atoi(optarg);
      break;
    }
  }

  if(N == 0) {
		if ( rank == root ){
      printf("Usage: mpirun -nn nodecount p3-allgather.exe -n N\n");
      printf("N is the the matrix size. \n\n");
		}
		return 1;
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


  double       inittime,totaltime;

  debug("begin MPI_Allgather");

  inittime = MPI_Wtime();

  MPI_Allgather(&sendbuff, partition, ATYPE_MPI,
                &recvbuff, partition, ATYPE_MPI,
                MPI_COMM_WORLD);

  totaltime = MPI_Wtime() - inittime;
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

  if(rank == 0) {
    printf("%d,%f\n",N, totaltime);
  }


  debug("cleaning up");

  free(vector);
  for(int i=0; i<N; i++) {
    free(matrix[i]);
  }

  free(matrix);

  MPI_Finalize();
  return 0;
}
