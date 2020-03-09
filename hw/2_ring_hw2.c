#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
//	printf ("I am %d of %d\n", rank, size);

	int token = -1;

	double start, end;

	start = MPI_Wtime();
	if (rank != 0) {
		MPI_Recv (&token, 1, MPI_INT, (rank - 1) % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf ("Process %d received token %d from process %d\n", rank, token, (rank - 1) % size);
	} else if (rank == 0) {
		token = 5;
	}
	MPI_Send (&token, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
	
	if (rank == 0) {
		MPI_Recv (&token, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf ("Process %d received token %d from process %d\n", rank, token, size - 1);
	}
	end = MPI_Wtime();

	MPI_Finalize();
	
	printf("Execution time %f for process of rank %d\n", end - start, rank);


	return 0;
}
	
