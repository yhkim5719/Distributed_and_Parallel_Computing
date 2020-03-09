#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 64

int main () {
	int nthreads, tid;
	int array[64];
	int i;
	int local_sum, sum = 0;

	#pragma omp parallel shared(sum) private(i, tid, local_sum)
	{
		local_sum = 0;
		tid = omp_get_thread_num();
		#pragma omp for 
		for (i = 0; i < N; i++) {
			array[i] = 0;
//			printf("Thread %d set, array[%d] = %d\n", tid, i, array[i]);
		}

		#pragma omp for
		for (i = 0; i < N; i++) {
			array[i] += 3*i;
//			printf("Thread %d set, array[%d] = %d\n", tid, i, array[i]);
		}
		#pragma omp for schedule(static, 1)
		for (i = 0; i < N; i++) {
			if (array[i] % 2 == 1) {
				local_sum += array[i];
			}
			printf("Thread %d add array[%d]'s\5 %d\t local_sum = %d\n", tid, i, array[i], local_sum);

		}
		#pragma omp critical
		sum += local_sum;
	}
	printf("\nThread %d: sum = %d\n\n", tid, sum);
}
