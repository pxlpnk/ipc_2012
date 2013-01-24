#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "util.h"

static const int root = 0;
static int rank, size;
static ATYPE *cor;

void arrayscan(ATYPE A[], int n, MPI_Comm comm) {
	int loc = prefixSumEx(A, n);
	int rcv = 0;

	MPI_Exscan(&loc, &rcv, 1, ATYPE_MPI, MPI_SUM, comm);

	uint i;
	for (i = 0; i < n; i++) {
		A[i] += rcv;
	}
}

int main(int argc, char *argv[])
{
	MPI_Comm comm = MPI_COMM_WORLD;
	uint n = (argv[1] != NULL) ? atoi(argv[1]) : 0; // TODO: error handling
	if (n == 0) {
		printf("N not given!\n");
		return EXIT_FAILURE;
	}

	ATYPE *arr = malloc(sizeof(ATYPE) * n);
	if (arr == NULL)
		return EXIT_FAILURE;

	arr = fillArr(arr, n);

	cor = malloc(sizeof(ATYPE) * n);
	if (cor == NULL)
		return EXIT_FAILURE;

	prefixSums(arr, n, false, cor);

	MPI_Init(&argc, &argv);

	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	double inittime = MPI_Wtime();

	/* distribute the array in smaller, approximately equal sized blocks of size ~n/size.
	 * the maximum error of block size is size-1 */
	if (rank != size-1)
		arrayscan(arr + (n/size) * rank, n/size, comm);
	else
		arrayscan(arr + (n/size) * rank, n - (n/size)*rank, comm);

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
