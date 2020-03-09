#include "mpi.h"
#include <stdio.h>

int rank, size;
char* name

void print() {
	printf("hello world\n");
	printf("I am %d of %d. \n", rank, size);
}

void ask() {
	printf("What is your name?\n");


int main (int argc, char* argv[]) {
//	int rank, size;
	MPI_Init(&argc, &argv);
//	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &size);
//	printf("hello world\n");
//	printf("I am %d of %d. \n", rank, size);


	print();
	MPI_Finalize();
	return 0;
}
	
