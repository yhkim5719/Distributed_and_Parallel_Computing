#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

int main (int argc, char* argv[]) {
	int rank, size;
	int n = 5;
	int a[5] = {0};
	int i;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf ("I am %d of %d, and my a[3] = %d \n", rank, size, a[3]);

	if (rank == 0) {
		for (i = 0; i < n; i++) {
			a[i] = i;
		}
	}
	MPI_Bcast(&a, n, MPI_INT, 0, MPI_COMM_WORLD);
	printf("After broadcast, process of rank %d has a[3] = %d \n", rank, a[3]); 
	MPI_Finalize();
	return 0;
}
