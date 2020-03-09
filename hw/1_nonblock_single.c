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
	MPI_Request ireq;
	MPI_Status istatus;
//	MPI_Status status;
	printf ("Process %d has number %f\n", rank, number);
	double start, end;
	start = MPI_Wtime();
	if (rank == 0) {
		number = 3.141592;
		MPI_Isend (&number, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &ireq);
		printf ("Process 0 sent %f to process 1\n", number);
		MPI_Wait(&ireq, &istatus);
	} else if (rank == 1) {
		MPI_Irecv (&number, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &ireq);
		MPI_Wait (&ireq, &istatus);
		printf ("Process 1 received %f\n", number);
	}
	printf ("Process %d has number %f\n", rank, number);
	end = MPI_Wtime();

	MPI_Finalize();
	printf("Execution time %f for process of rank %d\n", end - start, rank);


	return 0;
}
	
