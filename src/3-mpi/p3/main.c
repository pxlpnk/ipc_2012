#include <stdio.h>
#include <stdlib.h>

// MPI header
#include <mpi.h>

#include "../../shared/util.h"
#include "main.h"
#include "allgather.h"

#define N 10


void matrix_vector_mult_ref(ATYPE **x, ATYPE *a, uint n, uint m, ATYPE *y) {
  for(uint i=0; i<n; i++) {
    y[i]=0;
    for(uint j=0; j<m; j++) {
        y[i] += x[i][j] * a[j];
    }
  }
}




int main(int argc, char** argv) {
  int rank, size;

  char name[MPI_MAX_PROCESSOR_NAME];
  int nlen;

  int	buffsize;
  uint sendbuff[N], recvbuff[N],buffsum;

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



  MPI_Init(&argc,&argv);

  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  MPI_Get_processor_name(name,&nlen);


  /* ======================================================== */
  /* Initialisation send and receive buffers */

  debug("Setting up send and receive buffers");

  buffsize = N;

  for(int i=0; i<N; i++) {
    recvbuff[i] = vector[i];
  }


  for(int i=0; i<N; i++) {
    sendbuff[i] = sendbuff[i] + vector[i]*2;
  }



  for (int i=0; i<N; i++){
    sendbuff[i] = 0;
    for (int k=0; k<N; k++) {
      sendbuff[i] = sendbuff[i] + matrix[i][k] * vector[k];
    }
  }


  debug("MPI_Allgather");

  MPI_Allgather(&sendbuff, buffsize, MPI_INT,
                &recvbuff, buffsize, MPI_INT,
                MPI_COMM_WORLD);


  for(int i=0; i<N; i++) {
    printf("%d ",recvbuff[i]);
  }


  /* for(int i=0; i<N; i++) { */
  /*   printf("%d ",reference[i]); */
  /* } */



  debug("Testing result");


  if (testResult(recvbuff, reference, N)) {
    debug("testresult: OK");
  } else {
    debug("testresult: FAILURE");
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
