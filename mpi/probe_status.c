#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

const int MAX = 100;

int main (int argc, char* argv[]) {
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf ("I am %d of %d.\n", rank, size);

	int numbers[MAX];
	int number_amount;

	if (rank == 1) {
		srand(time(NULL));
		number_amount = (rand() / (float)RAND_MAX) * MAX;

		MPI_Send(numbers, number_amount, MPI_INT, 0, 10, MPI_COMM_WORLD);
		printf("Process 1 sent %d numbers to Process 0.\n", number_amount);
	} else if (rank == 0) {
		MPI_Status status;
		MPI_Recv(numbers, MAX, MPI_INT, 1, 10, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &number_amount);

		printf("Process 0 received %d numbers from 1. Message Source = %d, tag = %d\n", number_amount, status.MPI_SOURCE, status.MPI_TAG);
	}
	MPI_Finalize();
	return 0;
}
