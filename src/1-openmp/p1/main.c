#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	double time = omp_get_wtime();

	#pragma omp parallel
	{
		int th_id = omp_get_thread_num();
		printf("Hello World from thread %d\n", th_id);
		#pragma omp barrier
		if (th_id == 0) {
			printf("There are %d threads\n",omp_get_num_threads());
		}
	}

	time = omp_get_wtime() - time;
	printf("%lf seconds.\n",time);
	return EXIT_SUCCESS;
}
