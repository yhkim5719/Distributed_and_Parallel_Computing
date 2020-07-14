#include <cuda.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct Data {
	float* x;
        float* y;
	int num_node;
} data;

data* read_data(const char* file) {
	data* d = NULL;
	FILE* f = fopen (file, "r");

	int num_node;
	if (fscanf(f, "%5d\n", &num_node) == 0) {
		printf("read inputfile failed.\n");
		return NULL;
	}
	printf("num_node = %d\n", num_node);
	
	d = (data*) malloc(sizeof(data));
	d->num_node = num_node;
	d->x = (float*)malloc(num_node * sizeof(float));
	d->y = (float*)malloc(num_node * sizeof(float));
	for (int i = 0; i < num_node; i++) {
		fscanf(f, "%f %f\n", &(d->x[i]), &(d->y[i]));
	}

	for (int i = 0; i < d->num_node; i++) {
		printf("%d: x = %f, y = %f\n", i, d->x[i], d->y[i]);
	}
	
	fclose(f);
	
	return d;
}		

__device__ float dist(const float x, const float y, const float cx, const float cy) {
	return sqrt(pow(x - cx, 2.0) + pow(y - cy, 2.0));
}
/*
//__device__
float dist(const float x, const float y, const float cx, const float cy) {
	return sqrtf(powf(x - cx, 2.0) + powf(y - cy, 2.0));
}
*/
__device__ void kmeans_kernel(const float* x, const float* y, const float* cx, const float* cy, const int num_nodes, const int num_cent, const int iter, float* cx_result, float* cy_result, int* result) {
	
	__shared__ bool done;
	__shared__ float tmp_cx[num_cent];
	__shared__ float tmp_cy[num_cent];
	__shared__ int tmp_label[num_node];
	__shared__ int count[num_cent];
	__shared__ float dist_c[num_cent];

	unsigned int id = blockIdx.x * blockDim.x + threadIdx.x;
	
	for (int i = 0; i < iter; i++) {
	// calculate the closest centroid
		done = true;
		for (int jj = 0; jj < num_cent; jj++) {
			count[jj] = 0;
		}
		float tmp_min = 9999;
		for (int jj = 0; jj < num_cent; jj++) {
			if (tmp_min >  dist(x[id], y[id], cx[jj], cy[jj])) {
				tmp_label[id] = jj;
				atomicAdd(count + jj, 1);
			}
		}
	// update centroid
	// calculate new centroids
	// add all x, all y of the same label, then divide by the nubmer of same label nodes.
		for (int jj = 0; jj < num_cent; jj++) {
			tmp_cx[jj] = cx[jj];
			tmp_cy[jj] = cy[jj];
			cx[jj] = 0;
			cy[jj] = 0;
		}
	
		for (int jj = 0; jj < num_cent; jj++) {
			if (tmp_label[id] == jj) {
				atomicAdd(&(cx[jj]), x[id]);
				atomicAdd(&(cy[jj]), y[id]);
			}
		}

		for (int jj = 0; jj < num_cent; jj++) {
			cx[jj] /= count[jj];
			cy[jj] /= count[jj];
		}

	// calculate distance from prev_centroid to cur_centroid
		for (int jj = 0; jj < num_cent; jj++) {
			dist_c[jj] = dist(cx[jj], tmp_cx[jj], cy[jj], tmp_cy[jj]);
			if (dist_c[jj] > 0.001) {
				done = false;
			}
		}

	// if the change of dist < 0.01, then break
	// cx_result = tmp_cx
	// cy_result = tmp_cy
	// result = tmp_label;
	//
		if (done) {
			for (int jj = 0; jj < num_cent; jj++) {
				cx_result[jj] = tmp_cx[jj];
				cy_result[jj] = tmp_cy[jj];
				result[id] = tmp_label[id];
			}
			break;
		}
	}
}

void kmeans(const float* x, const float* y, const float* cx, const float* cy, const int num_nodes, const int num_cent, const int iter, float* cx_result, float* cy_result, int* result) {   				// memory alloc and hac execute
	
	int Dim = num_nodes;				// TODO

	float* d_in_x;						// for cudamalloc in 'nodes data'
	float* d_in_y;
	float* d_in_cx;
	float* d_in_cy;
	float* d_out_cx_result;						// result centroid
	float* d_out_cy_result;
	int* d_out_label_result;

	cudaMalloc((void**) &d_in_x, num_nodes * sizeof(float));				
	cudaMalloc((void**) &d_in_y, num_nodes * sizeof(float));				
	cudaMalloc((void**) &d_in_cx, num_cent * sizeof(float));				
	cudaMalloc((void**) &d_in_cy, num_cent * sizeof(float));				
	cudaMalloc((void**) &d_out_cx_result, num_cent * sizeof(float));				
	cudaMalloc((void**) &d_out_cy_result, num_cent * sizeof(float));				
	cudaMalloc((void**) &d_out_label_result, num_nodes * sizeof(int));				
	
	cudaMemcpy(d_in_x, x, num_nodes * sizeof(float), cudaMemcpyHostToDevice);	// node memcpy
	cudaMemcpy(d_in_y, y, num_nodes * sizeof(float), cudaMemcpyHostToDevice);		// centroid memcpy
	cudaMemcpy(d_in_cx, cx, num_cent * sizeof(float), cudaMemcpyHostToDevice);	// node memcpy
	cudaMemcpy(d_in_cy, cy, num_cent * sizeof(float), cudaMemcpyHostToDevice);		// centroid memcpy
	
	kmeans_kernel<<<1, Dim>>>(d_in_x, d_in_y, d_in_cx, d_in_cy, num_nodes, num_cent, iter, d_out_cx_result, d_out_cy_result, d_out_result);				// hac <<< ?, ? >>> (N, in, out);
	
	cudaMemcpy(result, d_out_label_result, num_nodes * sizeof(int), cudaMemcpyDeviceToHost);	// cuda memcopy d to h 
	cudaMemcpy(cx_result, d_out_cx_result, num_cent * sizeof(float), cudaMemcpyDeviceToHost);
	cudaMemcpy(cy_result, d_out_cy_result, num_cent * sizeof(float), cudaMemcpyDeviceToHost);

	cudaFree(d_in_x);
	cudaFree(d_in_y);	
	cudaFree(d_in_cx);
	cudaFree(d_in_cy);
	cudaFree(d_out_cx_result);	
	cudaFree(d_out_cy_result);	
	cudaFree(d_out_label_result);	

	return;
}

// TODO
void free_data(data* d) {

}

// TODO
void print_result (const float* cx, const float* cy, const int* label) {

}

int main (int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s <nodes file> <centroids file>", argv[0]);
		return 0;
	} else {
		data* d = read_data(argv[1]);
		data* c = read_data(argv[2]);
	
		float* cx_result = (float*)malloc(c->num_node * sizeof(float));;
		float* cy_result = (float*)malloc(c->num_node * sizeof(float));;
		int* result = (int*) malloc(d->num_node * sizeof(int));
		int iter = 100;

		if (d->num_node && c->num_node) {
			printf("Let's do CLUSTER!\n");
			kmeans(d.x, d.y, c.x, c.y, num_nodes, num_cent, iter, cx_result, cy_result, result);		// cuda
			
			printf("print the result!\n");
			
//			free(d);
//			free(c);
			free(cx_result);
			free(cy_result);
			free(result);
		} else {
			printf("empty file.\n");
		}
	}
	return 0;
}


