#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <mpi.h>
#include "util.h"

typedef enum algo {native, custom} algo_t;
static const char *algo2str[] = {
	"native",
	"custom"
};

void arrayscan(ATYPE A[], uint n, int rank, int size, MPI_Comm comm, algo_t algo) {
	/* distribute the array in smaller, approximately equal sized blocks of size ~n/size.
	 * the maximum error of block size is size-1 */
	const uint block_size = (rank != size-1) ? n/size : n - (n/size)*rank;
	ATYPE *block = A + (n/size) * rank;
	ATYPE block_sum = prefixSumEx(block, block_size);
	ATYPE preprefix = 0;

	if (algo == native)
		MPI_Exscan(&block_sum, &preprefix, 1, ATYPE_MPI, MPI_SUM, comm);
	else {
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
	}
	for (uint i = 0; i < block_size; i++) {
		block[i] += preprefix;
	}
}

int main(int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	const int root = 0;
	const MPI_Comm comm = MPI_COMM_WORLD;
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	FILE *f = NULL;

	char opt;
	uint n = 0;
	algo_t algo = custom;
	static const char optstring[] = "n:a:f:";
	static const struct option long_options[] = {
		{"n",			1, NULL, 'n'},
		{"file",		1, NULL, 'f'},
		{NULL,			0, NULL, 0},
	};
	while ((opt = getopt_long(argc, argv, optstring, long_options, NULL)) != EOF) {
		switch (opt) {
		case 'n':
			n = atoi(optarg); // TODO: error handling
			break;
		case 'f':
			f = fopen(optarg,"a");
			if (f == NULL) {
				mpi_printf(root, "Could not open log file '%s': %s\n", optarg, strerror(errno));
				ret = EXIT_FAILURE;
				goto out_mpi;
			}
			break;
		case 'a':
			if (strcmp("native", optarg) == 0) {
				mpi_printf(root, "Using native/MPI implementation of Exscan\n");
				algo = native;
			} else if (strcmp("custom", optarg) == 0) {
				mpi_printf(root, "Using custom implementation of Exscan\n");
				algo = custom;
			} else
				mpi_printf(root, "Warning: unknown algorithm %s, using default.\n", optarg);
			break;
		default:
			ret = EXIT_FAILURE;
			goto out_mpi;
			break;
		}
	}

	if (n == 0) {
		mpi_printf(root, "N not given!\n");
		ret = EXIT_FAILURE;
		goto out_mpi;
	}

	ATYPE *arr = malloc(sizeof(ATYPE) * n);
	if (arr == NULL) {
		ret = EXIT_FAILURE;
		goto out_mpi;
	}

	arr = fillArr(arr, n);

	ATYPE *cor = malloc(sizeof(ATYPE) * n);
	if (cor == NULL) {
		ret = EXIT_FAILURE;
		goto out_arr;
	}

	prefixSums(arr, n, false, cor);

	double inittime = MPI_Wtime();

	arrayscan(arr, n, rank, size, comm, algo);

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

	if (rank == 0) {
		if (n < 100)
			printArrs(cor, arr, n);

		printf("Correct? ");
		if (memcmp(cor, arr, sizeof(ATYPE) * n) == 0) {
			printf("yes\n");
			if (f != NULL)
				fprintf(f,"%s,%d,%d,%lf\n", algo2str[algo], size, n , totaltime);
		} else {
			printf("no\n");
			ret = EXIT_FAILURE;
		}
	}

	free(cor);
out_arr:
	free(arr);
out_mpi:
	MPI_Finalize();
	if (f != NULL)
		fclose(f);
	return ret;
}
