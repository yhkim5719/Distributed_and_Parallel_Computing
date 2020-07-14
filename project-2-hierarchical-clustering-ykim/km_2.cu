#include <cuda.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define N 64
#define K 3

#define THPERBLOCK 32
#define ITER 100

typedef struct Data {
	float* x;
        float* y;
} data;

data* read_data(const char* file) {
	data* d = NULL;
	FILE* f = fopen (file, "r");

/*
	int num_node;
	if (fscanf(f, "%5d\n", &num_node) == 0) {
		printf("read inputfile failed.\n");
		return NULL;
	}
	printf("num_node = %d\n", num_node);
*/	
	d = (data*) malloc(sizeof(data));
	d->x = (float*)malloc(N * sizeof(float));
	d->y = (float*)malloc(N * sizeof(float));
	for (int i = 0; i < N; i++) {
		fscanf(f, "%f %f\n", &(d->x[i]), &(d->y[i]));
	}

	printf("Point --------------------------------------\n");
	for (int i = 0; i < N; i++) {
		printf("%d: x = %f, y = %f\n", i, d->x[i], d->y[i]);
	}
	printf("\n");
	
	fclose(f);
	
	return d;
}		

data* read_cent(const char* file) {
	data* d = NULL;
	FILE* f = fopen (file, "r");

	d = (data*) malloc(sizeof(data));
	d->x = (float*)malloc(K * sizeof(float));
	d->y = (float*)malloc(K * sizeof(float));
	for (int i = 0; i < K; i++) {
		fscanf(f, "%f %f\n", &(d->x[i]), &(d->y[i]));
	}

	printf("Cluster location ---------------------------\n");
	for (int i = 0; i < K; i++) {
		printf("%d: x = %f, y = %f\n", i, d->x[i], d->y[i]);
	}
	printf("\n");
	
	fclose(f);
	
	return d;
}		

float dist_err(const float x, const float y, const float cx, const float cy) {
	return sqrt(pow(x - cx, 2.0) + pow(y - cy, 2.0));
}

float calculate_err(float* prev_cx, float* prev_cy, float* cx_result, float* cy_result) {
	float err = 0;
	for (int i = 0; i < K; i++) {
		err += dist_err(prev_cx[i], prev_cy[i], cx_result[i], cy_result[i]);
		prev_cx[i] = cx_result[i];
		prev_cy[i] = cy_result[i];
	}
	err /= K;

	return err;
}

__device__ float dist(const float x, const float y, const float cx, const float cy) {
	return sqrtf(powf(x - cx, 2.0) + powf(y - cy, 2.0));
}

__global__ void cluster_assign(const float* x, const float* y, float* cx, float* cy, int* cluster_label) {
	
	const unsigned int id = blockIdx.x * blockDim.x + threadIdx.x;
	
	if (id >= N) {
		return;
	}

	float min_dist = 99999999;
	int closest_cent = 0;

	for (int i = 0; i < K; i++) {
		float distance = dist(x[id], y[id], cx[i], cy[i]);

		if (distance < min_dist) {
			min_dist = distance;
			closest_cent = i;
		}
	}

	cluster_label[id] = closest_cent;

}

__global__ void cent_update(const float* x, const float* y, float* cx, float* cy, int* cluster_label, int* cluster_size) {
	unsigned int id = blockIdx.x * blockDim.x + threadIdx.x;
	
	if (id >= N) {
		return;
	}

	const int s_id = threadIdx.x;
	
	__shared__ float s_x[THPERBLOCK];
	__shared__ float s_y[THPERBLOCK];
	__shared__ int s_cluster_label[THPERBLOCK];

	s_x[s_id] = x[id];
	s_y[s_id] = y[id];
	s_cluster_label[s_id] = cluster_label[id];

	__syncthreads();

	if (s_id == 0) {
		float cent_x_sum[K] = { 0 };
		float cent_y_sum[K] = { 0 };
		int cent_cluster_size[K] = { 0 };

		for (int i = 0; i < blockDim.x; i++) {
			int cluster_id = s_cluster_label[i];
			cent_x_sum[cluster_id] += s_x[i];
			cent_y_sum[cluster_id] += s_y[i];
			cent_cluster_size[cluster_id]++;
		}

		for (int i = 0; i < K; i++) {
			atomicAdd(&cx[i], cent_x_sum[i]);
			atomicAdd(&cy[i], cent_y_sum[i]);
			atomicAdd(&cluster_size[i], cent_cluster_size[i]);
		}
	}
	
	__syncthreads();

	if (id < K) {
		cx[id] = cx[id] / cluster_size[id];
		cy[id] = cy[id] / cluster_size[id];
	}
}
	

void print_result (const float* cx, const float* cy, const int* label) {
	printf("\t");
	for (int i = 0; i < N; i++) {
		printf("%3d", i);
	}
	printf("\nLabel : ");
	for (int i = 0; i < N; i++) {
		printf("%3d", label[i]);
	}
	for (int i = 0; i < K; i++) {
		printf("\ncentroid %d\t :%5f %5f", i, cx[i], cy[i]);
	}
	printf("\n");
}

int main (int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s <nodes file> <centroids file>\n", argv[0]);
		return 0;
	} else {
		data* d = read_data(argv[1]);
		data* c = read_cent(argv[2]);
		
		float* cx_result = (float*)malloc(K * sizeof(float));
		float* cy_result = (float*)malloc(K * sizeof(float));
		int* result = (int*) malloc(N * sizeof(int));
		
		printf("Let's do CLUSTER!\n");
			
		float* d_x;						// for cudamalloc in 'nodes data'
		float* d_y;
		int* d_cluster_label;
	
		float* d_cx;
		float* d_cy;
		float* d_cx_result;						// result centroid
		float* d_cy_result;
		int* d_cluster_size;

		cudaMalloc((void**) &d_x, N * sizeof(float));				
		cudaMalloc((void**) &d_y, N * sizeof(float));				
		cudaMalloc((void**) &d_cluster_label, N * sizeof(int));				
	
		cudaMalloc((void**) &d_cx, K * sizeof(float));				
		cudaMalloc((void**) &d_cy, K * sizeof(float));				
		cudaMalloc((void**) &d_cx_result, K * sizeof(float));				
		cudaMalloc((void**) &d_cy_result, K * sizeof(float));				
		cudaMalloc((void**) &d_cluster_size, K * sizeof(int));				
	
		cudaMemcpy(d_x, d->x, N * sizeof(float), cudaMemcpyHostToDevice);	// node memcpy
		cudaMemcpy(d_y, d->y, N * sizeof(float), cudaMemcpyHostToDevice);		// centroid memcpy
		cudaMemcpy(d_cx, c->x, K * sizeof(float), cudaMemcpyHostToDevice);	// node memcpy
		cudaMemcpy(d_cy, c->y, K * sizeof(float), cudaMemcpyHostToDevice);		// centroid memcpy
	
		int cur_iter = 1;
		
		float err = 1;
		float* prev_cx = (float*)malloc(K * sizeof(float));
		float* prev_cy = (float*)malloc(K * sizeof(float));
		for (int i = 0; i < K; i++) {
			prev_cx[i] = c->x[i];
			prev_cy[i] = c->y[i];
		}
			
		while (cur_iter < ITER) {
	
			cluster_assign<<<(N + THPERBLOCK - 1)/THPERBLOCK, THPERBLOCK>>>(d_x, d_y, d_cx, d_cy, d_cluster_label);
		
//			cudaMemcpy(cx_result, d_cx, K * sizeof(float), cudaMemcpyDeviceToHost);
//			cudaMemcpy(cy_result, d_cy, K * sizeof(float), cudaMemcpyDeviceToHost);

			
			cudaMemset(d_cx, 0.0, K * sizeof(float));
			cudaMemset(d_cy, 0.0, K * sizeof(float));
			cudaMemset(d_cluster_size, 0, K * sizeof(int));
			
			cent_update<<<(N + THPERBLOCK - 1)/THPERBLOCK, THPERBLOCK>>>(d_x, d_y, d_cx, d_cy, d_cluster_label, d_cluster_size);
			
			cudaMemcpy(cx_result, d_cx, K * sizeof(float), cudaMemcpyDeviceToHost);
			cudaMemcpy(cy_result, d_cy, K * sizeof(float), cudaMemcpyDeviceToHost);
			
			for (int i = 0; i < K; i++) {
				printf("Iter %d:\tcent %d\t x:\t%f\ty:\t%f\n", cur_iter, i, cx_result[i], cy_result[i]);
			}
			printf("\n");

			err = calculate_err(prev_cx, prev_cy, cx_result, cy_result);
			if (err < 0.0001) {
				break;
			}
			cur_iter++;
		}
	
		cudaMemcpy(result, d_cluster_label, N * sizeof(int), cudaMemcpyDeviceToHost);	// cuda memcopy d to h 
//		cudaMemcpy(cx_result, d_cx_result, K * sizeof(float), cudaMemcpyDeviceToHost);
//		cudaMemcpy(cy_result, d_cy_result, K * sizeof(float), cudaMemcpyDeviceToHost);
	
		cudaFree(d_x);
		cudaFree(d_y);	
		cudaFree(d_cluster_label);	
	
		cudaFree(d_cx);
		cudaFree(d_cy);
		cudaFree(d_cx_result);	
		cudaFree(d_cy_result);	
		cudaFree(d_cluster_size);	
		
		printf("print the result!\n");
		
		print_result(cx_result, cy_result, result);
	
//		free(d);
//		free(c);
		free(cx_result);
		free(cy_result);
		free(result);
		free(prev_cx);
		free(prev_cy);
	}
	return 0;
}

/*
// TODO
void free_data(data* d) {
}
*/
