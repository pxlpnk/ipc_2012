#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <omp.h>
#include "algos.h"
#include "util.h"

typedef void (*scan_func_t)(ATYPE *x, uint n, uint *opsp);

double time_scan_func(scan_func_t f, ATYPE **x, uint n, uint *opsp) {
	ATYPE *arr = *x;
	double time = omp_get_wtime();
	if (f == hillis)
		hillis(x, n, opsp);
	else
		f(arr, n, opsp);
	return omp_get_wtime() - time;
}

int main (int argc, char *argv[]) {
	enum id {none = -1, alg, proc, en};
	char opt;
	int option_index;
	uint t = 0;
	uint n = 0;
	FILE *f = NULL;
	enum id id = none;
	scan_func_t func = seq;
	char *func_name = "seq";
	static const char optstring[] = "n:a:f:i:";
	static const struct option long_options[] = {
		{"nproc",		1, NULL, 't'},
		{"file",		1, NULL, 'f'},
		{"id",			1, NULL, 'i'},
		{NULL,			0, NULL, 0},
	};
	int ret = 0;

	while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != EOF) {
		switch (opt) {
		case 't':
			t = atoi(optarg); // TODO: error handling
			break;
		case 'n':
			n = atoi(optarg); // TODO: error handling
			break;
		case 'i':
			if (strcmp(optarg, "a") == 0)
				id = alg;
			else if (strcmp(optarg, "p") == 0)
				id = proc;
			else if (strcmp(optarg, "n") == 0)
				id = en;
			else {
				fprintf(stderr, "Invalid ID: %s\n", optarg);
				usage_abort();
			}
			break;
		case 'f':
			f = fopen(optarg,"a");
			if (f == NULL) {
				fprintf(stderr, "Could not open log file '%s': %s\n", optarg, strerror(errno));
				usage_abort();
			}
			break;
		case 'a':
			if (strcmp(optarg, "seq") == 0) {
				func = seq;
				func_name = "seq";
			} else if (strcmp(optarg, "auxarr") == 0) {
				func = auxarr;
				func_name = "auxarr";
			} else if (strcmp(optarg, "inplace") == 0) {
				func = inplace;
				func_name = "inplace";
			} else if (strcmp(optarg, "hillis") == 0) {
				func = hillis;
				func_name = "hillis";
			} else if (strcmp(optarg, "reduction") == 0) {
				func = reduction;
				func_name = "reduction";
			} else
				fprintf(stderr, "Warning: unknown algorithm %s, using default.\n", optarg);
			break;
		default:
			usage_abort();
			break;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Warning: Extra parameters found.\n");
	}

	if (n == 0) {
		fprintf(stderr, "Legal array size n required!\n");
		usage_abort();
	}

	if (f != NULL && id == none) {
		fprintf(stderr, "Legal ID required!\n");
		usage_abort();
	}

	if (t > 0) {
		omp_set_num_threads(t);
	}


	ATYPE *arr = malloc(sizeof(ATYPE) * n);
	if (arr == NULL)
		return EXIT_FAILURE;

	arr = fillArr(arr, n);

	ATYPE *cor = malloc(sizeof(ATYPE) * n);
	if (cor == NULL)
		return EXIT_FAILURE;

	prefixSums(arr, n, false, cor);

	printf("Using %s algorithm\n", func_name);
	uint ops = 0;
	double totaltime = time_scan_func(func, &arr, n, &ops);

	printf("%d additions of array elements executed in %lf seconds by 1 thread\n", ops, totaltime);

	if (n < 100)
		printArrs(cor, arr, n);
	else
		printArrsElem(cor, arr, n-1);

	ret = memcmp(cor, arr, sizeof(ATYPE) * n);

	printf("Correct? ");
	if (ret == 0) {
		printf("yes\n");
		if (f != NULL) {
			switch (id) {
				case alg:
					fprintf(f, "%s,%f\n", func_name, totaltime);
					break;
				case proc:
					fprintf(f, "%d,%f\n", t, totaltime);
					break;
				case en:
					fprintf(f, "%d,%f\n", n, totaltime);
					break;
				default:
					usage_abort();
			}
		}
	} else {
		printf("no\n");
		ret = EXIT_FAILURE;
	}

	free(cor);
	free(arr);
	if (f != NULL)
		fclose(f);

	return ret;
}

