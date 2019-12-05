#include "mpi.h"
#include <stdio.h>

int main (int argc, char* argv[]) {
	
	int rank, size;
	MPI_Init(&argc, &argv);
	printf("hello world\n");
	MPI_Group group_world, even_group, odd_group;
	int i, p, Neven, Nodd, members[20], ierr;
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_group(MPI_COMM_WORLD, &group_world);
	Neven = (p+1)/2;
	Nodd = p - Neven;
	for (i = 0; i < Neven; i++) {
		members[i] = 2 * i;
	}
	MPI_Group_incl(group_world, Neven, members, &even_group);
	MPI_Group_incl(group_world, Nodd, members, &odd_group);

	MPI_Comm even_comm, odd_comm;
	MPI_Comm_create(MPI_COMM_WORLD, even_group, &even_comm);
	i = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		i = 10;
	}
	MPI_Bcast(&i, 1, MPI_INT, 0, even_comm);
//	MPI_Bcast(&i, 1, MPI_INT, 0, odd_comm);

	printf("process %d has value %d\n", rank, i);

	MPI_Finalize();
	return 0;
}
	
