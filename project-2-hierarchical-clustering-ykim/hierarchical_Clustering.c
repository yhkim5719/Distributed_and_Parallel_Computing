// #include <cuda.h>
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
	
	*nodes = malloc(num_node*sizeof(node));
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
			free(nodes);
		} else {
			printf("empty file.\n");
		}
	}
	printf("print the result!\n");

	return 0;
}
