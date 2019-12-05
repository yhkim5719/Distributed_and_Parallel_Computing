#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

int main (int argc, char** argv) {
        
	int rank, gsize;
	int check_init;			// to check if a process is an initiator or not
        
	MPI_Comm graph1, graph2;
	
	int init[2] = {0, 3};		// set initiators TODO to change initiators and entities, change this line.

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &gsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;		// MPI Status

	int i, j;	
	int recv_tok_count = 0;
        int recv_rec_count = 0;		// to count the # of received token and reckon
	int num_neighbor;		
	int parent;
	int recv_tag, recv_from;	
	int curP;			// Which process dominates this process
	int Msg;			// msg from Process, which will be the rank of process, reckon msg
	int candidate;			// candidate process
	int topo_type;			// to check a graph topology
	// TODO to test with other graphs, modify this part
	// Example 2: on the powerpoint
	// creating nodes and edges
        //int index[5] = {1, 5, 7, 10, 12};
        //int edges[12] = {1, 0, 2, 3, 4, 3, 1, 2, 1, 4, 1, 3};	
	// end creating nodes and edges
	// Example 3: on the powerpoint
	// creating nodes and edges
	int index[5] = {3, 7, 9, 13, 16};
	int edges[16] = {1, 3, 4, 0, 2, 3, 4, 1, 3, 0, 1, 2, 4, 0, 1, 3};	
	// end creating nodes and edges
//	int *index, *edges;		// for the ring topology TODO 
	int *outindex, *outedges;	// to check a graph topology
	
	if (rank == 0) {	
       		printf("Initiators: ");
		for (i = 0; i < sizeof(init)/sizeof(int); i++) {
			if (i == sizeof(init)/sizeof(int) - 1) {
				printf(" %d\n", init[i]);
			} else {
				printf(" %d,", init[i]);
			}
		}
	}

	// create a graph
	if (gsize >= 3) {
		// TODO to test with ring topology, modify this part
		// Example 1: a simple ring graph
		// start creating nodes and edges
/*
		index = (int*)malloc(gsize * sizeof(int));
		edges = (int*)malloc(2 * gsize * sizeof(int));
		if (!index || !edges) {
			printf("Unable to allocate %d words for index or edges\n", 3 * gsize);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		index[0] = 2;
		for (i = 0; i < gsize; i++) {
			index[i] = 2 + index[i - 1];
		}
		j = 0;
		for (i = 0; i < gsize; i++) {
			edges[j++] = (i - 1 + gsize) % gsize;
			edges[j++] = (i + 1) % gsize;
		}
*/
		MPI_Graph_create(MPI_COMM_WORLD, gsize, index, edges, 0, &graph1);
/*		
		MPI_Comm_dup(graph1, &graph2);
		
		MPI_Topo_test(graph2, &topo_type);
		if(topo_type != MPI_GRAPH) {
			printf("This is not a graph.\n");
		} else {
			int nnodes, nedges;
			MPI_Graphdims_get(graph2, &nnodes, &nedges);
     			outindex = (int*)malloc(gsize * sizeof(int));
			outedges = (int*)malloc(2 * gsize * sizeof(int));
			
			MPI_Graph_get(graph2, 5, 12, outindex, outedges);
		}
		if (rank == 0) {
			for (i = 0; i < gsize; i++) {
				printf("index[%d] = %d, \tedges[%d] = %d,\t%d \n", i, index[i], i, edges[2*i], edges[2*i + 1]);
			}
		}
		free(outindex);
		free(outedges);
*/
	}

	// find the process is an initiator
	for (i = 0; i < sizeof(init)/sizeof(int); i++) {
		if (rank == init[i]) {
			check_init = 1;
			break;
		} else {
			check_init = 0;
		}
	}
	
	// every processes' curP = -1
	curP = -1;

	// get the # of neighbors ( # of edges, degree )
	if (rank == 0) {
		num_neighbor = index[0];
//		printf("num_neighbor of P0 = %d\n", num_neighbor);
	} else {
		num_neighbor = index[rank] - index[rank - 1];
	}

	// create edges_recv_reckon and edges_recv_tok
	int edges_recv_reckon[num_neighbor];
	int edges_recv_tok[num_neighbor];
	for (i = 0; i < num_neighbor; i++) {
		if (rank == 0) {
			edges_recv_reckon[i] = edges[i];
			printf("%d's edges[%d] = %d\n", rank, i, edges_recv_reckon[i]);	//TODO test
		} else {
			edges_recv_reckon[i] = edges[index[rank - 1] + i];
			printf("%d's edges[%d] = %d\n", rank, i, edges_recv_reckon[i]);	//TODO test
		}
	}
	for (i = 0; i < num_neighbor; i++) {
		if (rank == 0) {
			edges_recv_tok[i] = edges[i];
		} else {
			edges_recv_tok[i] = edges[index[rank - 1] + i];
		}
	}
	
	// if (process is an initiator)
	if (check_init) {
		curP = rank;
		Msg = rank;
		printf("Process %d is an initiator!\n", rank);		
		if (rank == 0) {
			for (i = 0; i < num_neighbor; i++) {
				MPI_Send(&Msg, 1, MPI_INT, edges[i], 0, MPI_COMM_WORLD);
				printf("\tS: P%d---tok%d-->P%d\n", rank, Msg, edges[i]);		
			}
		} else {
			for (i = 0; i < num_neighbor; i++) {
				MPI_Send(&Msg, 1, MPI_INT, edges[index[rank - 1] + i], 0, MPI_COMM_WORLD);
				printf("\tS: P%d---tok%d-->P%d\n", rank, Msg, edges[index[rank - 1] + i]);		
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);

	while (recv_rec_count < num_neighbor) {					// while (# of received reckon msgs < # of neighbor processes) {
		MPI_Recv(&Msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);	// receive msg from One neighbor process
		recv_tag = status.MPI_TAG;	
		recv_from = status.MPI_SOURCE;
		if (recv_tag == 1) {							// if (msg == reckon) {
			printf("\tR: P%d<--rec%d---P%d\n", rank, Msg, recv_from);
			if (recv_rec_count == 0) {						
				for (i = 0; i < num_neighbor; i++) {		
					if (rank == 0) {
						MPI_Send(&Msg, 1, MPI_INT, edges[i], 1, MPI_COMM_WORLD);
						printf("\tS: P%d---rec%d-->P%d\n", rank, Msg, edges[i]);		
					} else {
						MPI_Send(&Msg, 1, MPI_INT, edges[index[rank - 1] + i], 1, MPI_COMM_WORLD);
						printf("\tS: P%d---rec%d-->P%d\n", rank, Msg, edges[index[rank - 1] + i]);		
					}
				}
			}
			for (i = 0; i < num_neighbor; i++) {				// recv_rec_count++ 
				if (edges_recv_reckon[i] == recv_from) {
					recv_rec_count++;
				}
			}
			candidate = curP;						// Leader = Candidate
		} else if (recv_tag == 0) {						// if tag == 0, msg == token
			if (Msg < curP) {						// if (P < curP) 
				printf("\tR: P%d<--tok%d---P%d : msg%d purged\n", rank, Msg, recv_from, Msg);	
			} else if (Msg == curP) {					// else if ( P == curP )
				printf("\tR: P%d<--tok%d---P%d\n", rank, Msg, recv_from);
				for (i = 0; i < num_neighbor; i++) {				//TODO change the edge value to -1 
					if (edges_recv_tok[i] == recv_from) {
						recv_tok_count++;					//TODO test
						edges_recv_tok[i] = -1;
					}
				}
				if (recv_tok_count == num_neighbor) {					// if ( every value in the array == -1)
					if (rank == curP) {				// if (this process has the same rank )
						for (i = 0; i < num_neighbor; i++) {	// send the reckon msg as a Candidate to neighbor processes except for Sender
							if (rank == 0) { // && edges[i] != recv_from) {
								MPI_Send(&Msg, 1, MPI_INT, edges[i], 1, MPI_COMM_WORLD);
								printf("\tS: P%d---rec%d-->P%d\n", rank, Msg, edges[i]);		
							} else if (rank != 0) { //  && edges[index[rank - 1] +i] != recv_from) {
								MPI_Send(&Msg, 1, MPI_INT, edges[index[rank - 1] + i], 1, MPI_COMM_WORLD);
								printf("\tS: P%d---rec%d-->P%d\n", rank, Msg, edges[index[rank - 1] + i]);		
							}
						}
					} else {
						MPI_Send(&Msg, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);	// else send a token of P to its Parent
						printf("\tS: P%d---tok%d-->P%d\n", rank, Msg, parent);		
					}
				}
			} else {								// if Msg > curP
				printf("\tR: P%d<--tok%d---P%d\n", rank, Msg, recv_from);
				curP = Msg;						// curP = Msg
				recv_tok_count = 0;
				for (i = 0; i < num_neighbor; i++) {
					if (rank == 0) {
						edges_recv_tok[i] = edges[i];
					} else {
						edges_recv_tok[i] = edges[index[rank - 1] + i];
					}
				}
				for (i = 0; i < num_neighbor; i++) {				//TODO change the edge value to -1 
					if (edges_recv_tok[i] == recv_from) {
						recv_tok_count++;					//TODO test
//						printf("%d's # of recv_tok = %d, the value = %d\n", rank, recv_tok_count, edges_recv_tok[i]); //TODO test 
						edges_recv_tok[i] = -1;
//						printf("now the value = %d\n", edges_recv_tok[i]); //TODO test
					}
				}
				parent = recv_from;					// Parent = P
				printf("P%d's parent = %d, and curP = %d\n", rank, parent, curP);	
				for (i = 0; i < num_neighbor; i++) {			// send token msgs of curP to neighbor processes except for Sender
					if (rank == 0 && edges[i] != recv_from) {
						MPI_Send(&Msg, 1, MPI_INT, edges[i], 0, MPI_COMM_WORLD);
						printf("\tS: P%d---tok%d-->P%d\n", rank, Msg, edges[i]);		
					} else if (rank != 0 && edges[index[rank - 1] +i] != recv_from) {
						MPI_Send(&Msg, 1, MPI_INT, edges[index[rank - 1] + i], 0, MPI_COMM_WORLD);
						printf("\tS: P%d---tok%d-->P%d\n", rank, Msg, edges[index[rank - 1] + i]);		
					}
				}
				if (recv_tok_count == num_neighbor) {					// if ( every value in the array == -1)
					if (rank == curP) {				// if (this process has the same rank )
						for (i = 0; i < num_neighbor; i++) {	// send the reckon msg as a Candidate to neighbor processes except for Sender
							if (rank == 0) { // && edges[i] != recv_from) {
								MPI_Send(&Msg, 1, MPI_INT, edges[i], 1, MPI_COMM_WORLD);
								printf("\tS: P%d---rec%d-->P%d\n", rank, Msg, edges[i]);		
							} else if (rank != 0) { //  && edges[index[rank - 1] +i] != recv_from) {
								MPI_Send(&Msg, 1, MPI_INT, edges[index[rank - 1] + i], 1, MPI_COMM_WORLD);
								printf("\tS: P%d---rec%d-->P%d\n", rank, Msg, edges[index[rank - 1] + i]);		
							}
						}
					} else {
						MPI_Send(&Msg, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);	// else send a token of P to its Parent
						printf("\tS: P%d---tok%d-->P%d\n", rank, Msg, parent);		
					}
				}
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (check_init) {
		if ( rank == candidate) {
			printf("I(process %d) am the leader!\n", rank);
		} else {
			printf("I(process %d) lost.\n", rank);
		}
	}
//	free(index);
//	free(edges);
	MPI_Comm_free(&graph1);
//	MPI_Comm_free(&graph2);
	MPI_Finalize();

	return 0;
}
