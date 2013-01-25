#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"


void arrayscan(ATYPE A[], uint n, int rank, int size, MPI_Comm comm) {
	/* distribute the array in smaller, approximately equal sized blocks of size ~n/size.
	 * the maximum error of block size is size-1 */
	const uint block_size = (rank != size-1) ? n/size : n - (n/size)*rank;
	ATYPE *block = A + (n/size) * rank;
	ATYPE block_sum = prefixSumEx(block, block_size);
	ATYPE preprefix = 0;
	for (uint k = 1; k < size; k <<= 1) {
		ATYPE tmp = block_sum + preprefix;
		mpi_printf(0, "k=%d. ", k);
		MPI_Request request;
		if (rank < size - k) { // initiate send to rank + k
			printf("%d sends %" ATYPEPRINT " to %d\n", rank, tmp, rank + k);
			MPI_Isend(&tmp, 1, ATYPE_MPI, rank + k, k, comm, &request);
		}
		if (rank >= k) { // rcv from rank - k
			MPI_Recv(&tmp, 1, ATYPE_MPI, rank - k, k, comm, MPI_STATUS_IGNORE);
			printf("%d received %" ATYPEPRINT " from %d\n", rank, tmp, rank - k);
		}
		if (rank < size - k) { // complete send
			MPI_Wait(&request, MPI_STATUS_IGNORE);
		}
		if (rank >= k) { // add to local
			preprefix = tmp + preprefix;
		}
	}
	printf("%d: preprefix %" ATYPEPRINT ", block_sum %" ATYPEPRINT "\n", rank, preprefix, block_sum);
	for (uint i = 0; i < block_size; i++) {
		block[i] += preprefix;
	}
}

int main(int argc, char *argv[])
{
	uint n = (argv[1] != NULL) ? atoi(argv[1]) : 0; // TODO: error handling
	if (n == 0) {
		printf("N not given!\n");
		return EXIT_FAILURE;
	}

	ATYPE *arr = malloc(sizeof(ATYPE) * n);
	if (arr == NULL)
		return EXIT_FAILURE;

	arr = fillArr(arr, n);

	ATYPE *cor = malloc(sizeof(ATYPE) * n);
	if (cor == NULL)
		return EXIT_FAILURE;

	prefixSums(arr, n, false, cor);

	int rank, size;
	const int root = 0;
	const MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	double inittime = MPI_Wtime();

	arrayscan(arr, n, rank, size, comm);

	MPI_Barrier(comm);
	double totaltime = MPI_Wtime() - inittime;

	/* gather results at root node */
	if (rank == root) {
		printf("time used: %f \n", totaltime);
		int rcount[size];
		int rdisp[size];
		uint i = 0;
		for (; i < size - 1; i++) {
			rcount[i] = (n/size) * i;
			rdisp[i] = (n/size) * i;
		}
		rcount[i] = n - (n/size)*rank;
		rdisp[i] = (n/size) * i;
		MPI_Gatherv(MPI_IN_PLACE, n/size, ATYPE_MPI, arr, rcount, rdisp, ATYPE_MPI, root, comm);
	} else if (rank == size - 1)
		MPI_Gatherv(arr + (n/size) * rank, n - (n/size)*rank, ATYPE_MPI, NULL, 0, 0, 0, root, comm);
	else
		MPI_Gatherv(arr + (n/size) * rank, n/size, ATYPE_MPI, NULL, 0, 0, 0, root, comm);

	MPI_Finalize();

	if (rank == 0) {
		if (n < 100)
			printArrs(cor, arr, n);

		printf("Correct? %s\n", memcmp(cor, arr, sizeof(ATYPE) * n) == 0 ? "yes" : "no");
	}

	return 0;
}
