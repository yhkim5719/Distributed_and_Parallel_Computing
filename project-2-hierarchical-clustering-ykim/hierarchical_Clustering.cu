//#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>

typedef struct coord_s {
	float x;
        float y;
} coord;

typedef struct node_s {
	coord coord;
	int label;
} node;

typedef struct cluster_s {
	int num_nodes;
	node* node;
} cluster;

void read_nodes(int num_nodes, node* nodes, FILE* f) {
	for (int i = 0; i < num_nodes; i++) {
		node* n = &(nodes[i]);
		if (fscanf(f, "%f %f\n", &(n->coord.x), &(n->coord.y))) {
			n->label = i;
			continue;
		} else {
			printf("read line %d failed.\n", i);
			return;
		}

	}
	for (int i = 0; i < num_nodes; i++) {
		printf("%d: x = %f, y = %f\n", nodes[i].label, nodes[i].coord.x, nodes[i].coord.y);
	}

}

int read_data(node** nodes, const char* file) {
	
	FILE* f = fopen (file, "r");

	printf("file opend.\n");	
	int num_node;
	if (fscanf(f, "%5d\n", &num_node) == 0) {
		printf("read inputfile failed.\n");
		return 0;
	}
	printf("num_node = %d\n", num_node);
	
	*nodes = (node*)malloc(num_node*sizeof(node));
	if (*nodes) {
		read_nodes(num_node, *nodes, f);
	//	free(nodes);
	} else {
		printf("malloc failed.\n");
		return 0;
	}
	
	fclose(f);
	printf("file closed.\n");
	
	return num_node;
}		

//__device__ 
float dist(const node* n1, const node* n2) {
	return sqrt(pow(n1->coord.x - n2->coord.x, 2) + pow(n1->coord.y - n2->coord.y, 2));
}

//__device__ 
void generate_dist_matrix(int num_nodes, node* nodes, float* dist_matrix) {
	for (int ii = 0; ii < num_nodes; ii++) {
		for (int jj = ii; jj < num_nodes; jj++) {
			if (ii == jj) {
				dist_matrix[ii * num_nodes + jj] = 0;
			} else {
				dist_matrix[ii * num_nodes + jj] = dist(nodes[ii], nodes[jj]);	// dist_matrix[ii][jj] = dist(ii, jj)
				dist_matrix[jj * num_nodes + ii] = dist_matrix[ii * num_nodes + jj];	//dist_matrix[jj][ii] = dist_matrix[ii][jj]
			}
		}
	}
	return dist_matrix;
}

//__device__
void print_matrix(int num_nodes, float* dist_matrix) {
	for (int ii = 0; ii < num_nodes; ii++) {
		for (int jj = ii; jj < num_nodes; jj++) {
			printf("%5f   ", dist_matrix[ii * num_nodes + jj]);
		}
		printf("\n");
	}
}

//__device__ 
void update_matrix_result(int num_iter, int num_nodes, int* closest, float* matrix, int* result) {
	result[num_iter * num_nodes] = num_iter;
}

//__device__
void find_closest(int num_nodes, int* closest, float* dist_matrix) {
	float closest_dist = 9999;
	for (int ii = 0; ii < num_nodes; ii++) {
		for (int jj = ii + 1; jj < num_nodes; jj++) {
			if (dist_matrix[ii * num_nodes + jj] == 0) {
				continue;
			}
			if (dist_matrix[ii * num_nodes + jj] < closest_dist) {
				closest[0] = ii;
				closest[1] = jj;
			}
		}
	}
}

//__global__ 
void hac(int num_nodes, node* nodes, int* result) {
	//__shared__
	float dist_matrix[num_nodes * num_nodes];
	//__shared__
	int closest[2];
	
	generate_dist_matrix(num_nodes, nodes, dist_matrix);	// compute dist matrix
	print_matrix(num_nodes, dist_matrix);
	
	int num_iter = 0;
	while (num_iter < num_nodes - 1) {			// while (# of cluster > 1) 
		find_closest(num_nodes, closest, dist_matrix);		// 	choose two clusters, a set of node(s), that have the closest distance
		update_matrix_result(num_iter, num_nodes, closest, dist_matrix, result);			// 	recompute dist matrix
									// 	every dist in one cluster = 0
									// 	dist outside the cluster = the shortest one between the node and an any node in the cluster
		num_iter++;
	}
}

/* 	TODO

//__host__ 
void call_hac(node* nodes, int num_nodes) {   				// memory alloc and hac execute
	node* d_in_nodes;						// for cudamalloc in 'nodes data'

	int* d_out_result;						// for cudamalloc out 'result'
	cudaMalloc((node**) &d_in_nodes, num_nodes * sizeof(node));				
	cudaMalloc((int**) &d_out_result, num_nodes * num_nodes * sizeof(int));
	cudaMemcpy(d_in_nodes, nodes, num_nodes * sizeof(node), cudaMemcpyHostToDevice);	// cuda memcopy h to d;
	
	hac<<<1, num_nodes>>>(num_nodes, nodes, d_out_result);				// hac <<< ?, ? >>> (N, in, out);
	
	cudaMemcpy(result, d_out_result, num_nodes * num_nodes * sizeof(int), cudaMemcpyDeviceToHost);	// cuda memcopy d to h 

	cudaFree(d_in_nodes);		// cuda free (in)
	cudaFree(d_out_result);		// cuda free (out)
}

*/

/*
void createDataset();
void printData(const Data& d);
__device__ double dist(const double* x, const double* y, int size);
*/

int main (int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s <input file>", argv[0]);
		return 0;
	} else {
		node *nodes = NULL;
		int num_nodes = read_data(&nodes, argv[1]);
		if (num_nodes) {
			printf("Let's do CLUSTER!\n");
			//mem_alloc
			//__global__ void hac(&nodes);		// cuda
			
			free(nodes);
		} else {
			printf("empty file.\n");
		}
	}
	if (num_nodes) {
		printf("print the result!\n");
	}

	return 0;
}
