#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
//	printf ("I am %d of %d\n", rank, size);

	double number = 0;

	double start, end;

	MPI_Status status;
	printf ("Process %d has number %f\n", rank, number);
	start = MPI_Wtime();
	if (rank == 0) {
		number = 2.718281;
		MPI_Send (&number, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD); 
		printf ("Process 0 sent %f to process 1\n", number);
	} else if (rank == 1) {
		MPI_Recv (&number, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf ("Process 1 received %f\n", number);
	}
	printf ("Process %d has number %f\n\n", rank, number);
	end = MPI_Wtime();

	MPI_Finalize();
	printf("Execution time %f for process of rank %d\n", end - start, rank);


	return 0;
}
	
