#include <stdio.h>
#include "mpi.h"

int main (int argc, char* argv[]) {
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf ("I am %d of %d.\n", rank, size);

	int number;
	if (rank == 0) {
		number = 717;
		MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("Process 0 sent sent number %d to Process 1.\n", number);
	} else if (rank == 1) {
		MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process 1 received number %d to Process 0.\n", number);
	}
	MPI_Finalize();
	return 0;
}
