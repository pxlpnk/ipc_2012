#include <stdio.h>
#include <stdlib.h>

// MPI header
#include <mpi.h>

#include "../../../shared/util.h"
#include "main.h"

#define REPLAYS 10

#define ROOT 0
#define N 1000


void matrix_vector_mult_ref(ATYPE *x, ATYPE *a, uint n, ATYPE *y) {

  for ( int i = 0 ; i < n ; ++i ){
    for ( int j = 0 ; j < n ; ++j ){
      y[j] = y[j] + x[i*n+j] * a[i];
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


/* ======================================================== */
/* distributing usinge scatterv */

void distribute_vector(ATYPE *root_vector, ATYPE *local_vector, int local_rank, int proc_size, long partition){
  int sendcounts[proc_size], displs[proc_size],i;
  int rest = N - (partition * ( proc_size - 1) );

  sendcounts[0] = partition;


  for ( i = 0 ; i < proc_size ; i++ ){
    sendcounts[i] = (i == proc_size - 1 ) ? rest : partition;
    displs[i] = i*partition;
  }

  if ( local_rank == ROOT ){
    MPI_Scatterv(&(root_vector[0]), sendcounts, displs, MPI_INT, &local_vector[0], partition, MPI_INT, ROOT, MPI_COMM_WORLD);
  }else{
    MPI_Scatterv(NULL, sendcounts, displs, MPI_INT, &local_vector[0], partition, MPI_INT, ROOT, MPI_COMM_WORLD);
  }

}

void distribute_matrix(ATYPE *root_matrix, ATYPE *local_matrix, int local_rank, int proc_size, long partition){
  int sendcounts[proc_size], displs[proc_size];
  ATYPE *sendbuffer=NULL;

  MPI_Datatype MPI_type, MPI_type2;


  int rest = N - (partition * ( proc_size - 1) );


  MPI_Type_vector(N, 1, N, MPI_INT, &MPI_type2);
  MPI_Type_create_resized( MPI_type2, 0, sizeof(ATYPE), &MPI_type);
  MPI_Type_commit(&MPI_type);


  for ( int i=0 ; i<proc_size ; ++i ){
    if ( i == proc_size - 1 ) {
      sendcounts[i] = rest;
    }
    else {
      sendcounts[i] = partition;
    }
    displs[i] = i*partition;
  }

  if ( local_rank == ROOT )
    sendbuffer = &(root_matrix[0]);

  MPI_Scatterv( sendbuffer, sendcounts, displs, MPI_type, &(local_matrix[0]), partition*N, MPI_INT, ROOT, MPI_COMM_WORLD );
  MPI_Type_free(&MPI_type);
}



ATYPE*  init_vector(long size, ATYPE value){
  long i;
  ATYPE *vector = (ATYPE*) malloc (sizeof(ATYPE) * size );
  if ( vector == NULL ){
    printf("Error: out of memory!\n");
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
  for ( i = 0l ; i < size ; ++i ){
    vector[i] = value;
  }
  return vector;
}

ATYPE* init_matrix(long size, ATYPE value){
  long i,j;
  ATYPE *array = (ATYPE*) malloc( sizeof(ATYPE) * size * size);

  if ( array == NULL ){
    printf("Error: out of memory!\n");
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }


  for ( i=0l ; i < size ; ++i ){
    for ( j=0l ; j < size ; ++j ){
      array[i*size + j] = j;
    }
  }
  return array;
}


void compute_reduce_scatter(ATYPE *matrix, ATYPE *vector, ATYPE *result, int local_rank, int proc_size , long n, long partition) {
  ATYPE *temp_result = init_vector(N,0);
  int recvcounts[proc_size];


  for( int i=0; i < proc_size; i++) {
    recvcounts[i] = N;
  }

  for ( int i = 0 ; i < partition ; ++i ){
    for ( int j = 0 ; j < N ; ++j ){
      temp_result[j] = temp_result[j] + matrix[i*n+j] * vector[i];
    }
  }
}



int main(int argc, char** argv) {
  int rank, size;

  char name[MPI_MAX_PROCESSOR_NAME];
  int nlen;

  MPI_Init(&argc,&argv);

  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  MPI_Get_processor_name(name,&nlen);

  /* ======================================================== */
  /* Initialisation matrix & vector */

  ATYPE *matrix = NULL;
  ATYPE *vector = NULL;


  if (rank == ROOT) {
    debug("Setting up ROOT data structures");
    matrix = init_matrix(N,1);
    vector = init_vector(N,1);
  }

  int colcnt =  N - (N/size ) * (size - 1 );
  int partition = N/size;

  ATYPE *local_matrix = NULL;
  local_matrix = (ATYPE*) malloc (sizeof(ATYPE) * N * colcnt);


  ATYPE *local_vector = NULL;
  local_vector = (ATYPE*) malloc (sizeof(ATYPE) * partition) ;


  ATYPE *reference = NULL;
  reference = init_vector(N,1);

  ATYPE *result = NULL;
  result = init_vector(N,1);

  if(rank == ROOT){
    debug("Comptuting reference");
    matrix_vector_mult_ref(matrix, vector, N, reference);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  printf("Rank: %d\n", rank);
  printf("size/rank: %d\n", (size/(rank+1)));
  printf("partition: %d\n", partition);

  /* ======================================================== */
  /* distributing matrix */

  for( int i=0; i< REPLAYS; i++) {

    distribute_vector(vector, local_vector, rank, size, partition);
    distribute_matrix(matrix, local_matrix, rank, size, partition);

    double       inittime,totaltime;

    debug("begin MPI_Reduce_scatter");
    MPI_Barrier(MPI_COMM_WORLD);
    inittime = MPI_Wtime();
    compute_reduce_scatter(local_matrix, local_vector, result, rank, size, N, partition);
    MPI_Barrier(MPI_COMM_WORLD);
    totaltime = MPI_Wtime() - inittime;
    debug("after MPI_Reduce_scatter");


    /* debug("Testing result"); */
    /* if (test_vector_part(result, recvbuff, (rank * partition) , partition)) { */
    /*   debug("testresult: OK"); */
    /* } else { */
    /*   debug("testresult: FAILURE"); */
    /*   debug("Result:"); */
    /*   printArray(recvbuff, N); */
    /*   debug("Reference:"); */
    /*   printArray(reference,N); */
    /* } */

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0) {
      printf("=> time used:");
      printf("%f \n",totaltime);
    }
  }

  debug("cleaning up");

  free(vector);

  free(matrix);

  MPI_Finalize();
  return 0;
}
