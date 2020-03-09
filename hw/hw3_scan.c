#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	
	int i;
	int idx_quantity = 0;
	for (i = 0; i < rank; i++) {
		srand (time(NULL));	
		idx_quantity += (rand() % 37);
	}
	idx_quantity %= 5;
	idx_quantity *= 2;
	idx_quantity++;

	int idx[2];
	idx[0] = idx_quantity;

	printf ("Process %d request: %d index(es)\n", rank, idx_quantity);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scan (&idx_quantity, &idx[1], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf ("Process %d will have index (%d ~ %d)\n", rank, idx[1] - idx[0] + 1, idx[1]);

	MPI_Finalize();

	return 0;
}
	
