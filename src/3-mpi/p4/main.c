#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"

#define root 0




void reference(ATYPE *a, uint n, uint r, ATYPE *b)  {
  ATYPE bucket[r];

  int i;
  for(i = 0; i < r; i++) bucket[i] = 0;
  for(i = 0; i < n; i++) bucket[a[i]] ++;

  for(i = 1; i < r; i++) bucket[i] += bucket[i-1];
  printArray(bucket,r);
  for(i = r-1; i > 0; i--) bucket[i] = bucket[i-1];
  bucket[0] = 0;
  for(i = 0; i < n; i++) b[bucket[a[i]]++] = a[i];
}

int main(int argc, char *argv[]) {
  int r = 6;
  int n = 10;
  ATYPE a[] = {0,2,1,3,4,2,1,5,4,5};
  ATYPE b[n];

  int rank, size;

  MPI_Init(&argc,&argv);
  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);


  if (rank == root)
    reference(a, n, r, b);


  int i;
  int m= n/size;

  /* create bucket locally */

  ATYPE B[r];

  const uint block_size = (rank != size-1) ? n/size : n - (n/size)*rank;

  printf("blocksize %d :rank %d\n",block_size, rank);


  ATYPE local_A[block_size];
  ATYPE local_B[r];

  int j = 0;
  for(i = rank * block_size; i < (rank + 1) * block_size; i++) {
    local_A[j] = a[i];
    j++;
  }

  for(i = 0; i < r; i++) local_B[i] = 0;
  for(i = 0; i < block_size; i++) local_B[local_A[i]] ++;


  ATYPE AllB[r];
  ATYPE RelB[r];

  MPI_Allreduce(&local_B, AllB, r, ATYPE_MPI, MPI_SUM, MPI_COMM_WORLD);
  MPI_Exscan(&local_B, RelB,r, ATYPE_MPI, MPI_SUM, MPI_COMM_WORLD);

  //  printArray(RelB, r);

  ATYPE temp[r];
  if ( rank > root) {
    for(int j=1; j< block_size; j++) {
      if (local_A[j] > 0) {
        local_A[ local_A[j] + RelB[ local_A[j]] + local_A[j-1] ] = local_A[j];
      }
    }

    printf("local_A\n");
    printArray(local_A, block_size);
  }





  MPI_Finalize();
  return 0;
}
