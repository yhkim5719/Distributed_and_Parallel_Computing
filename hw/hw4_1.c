#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 64

int main () {
	int nthreads, tid;
	int array[64];
	int i;

	#pragma omp parallel private(i, tid)
	{
		tid = omp_get_thread_num();
		#pragma omp for 
		for (i = 0; i < N; i++) {
			array[i] = 0;
			printf("Thread %d:\tarray[%d] = %d\n", tid, i, array[i]);
		}
	}
}
