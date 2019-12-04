#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

int main (int argc, char* argv[]) {
	int i, j;
	int gsize, rank;
	int topo_type;
	int *index, *edges, *outindex, *outedges;
	MPI_Comm graph1, graph2;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &gsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (gsize >= 3) {
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
		MPI_Graph_create(MPI_COMM_WORLD, gsize, index, edges, 0, &graph1);

		MPI_Comm_dup(graph1, &graph2);

		MPI_Topo_test(graph2, &topo_type);
		if(topo_type != MPI_GRAPH) {
			printf("This is not a graph.\n");
		} else {
			int nnodes, nedges;
			MPI_Graphdims_get(graph2, &nnodes, &nedges);
//			printf("This is a graph.\n");
			outindex = (int*)malloc(gsize * sizeof(int));
			outedges = (int*)malloc(2 * gsize * sizeof(int));

			MPI_Graph_get(graph2, gsize, 2*gsize, outindex, outedges);
		}
		if (rank == 0) {
			for (i = 0; i < gsize; i++) {
				printf("index[%d] = %d, \tedges[%d] = %d,\t%d \n", i, index[i], i, edges[2*i], edges[2*i + 1]);
			}
		}
		free(outindex);
		free(outedges);

	}
	free(index);
	free(edges);
	MPI_Comm_free(&graph1);
	MPI_Comm_free(&graph2);

	MPI_Finalize();
	return 0;
}


