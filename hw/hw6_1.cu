#include <cuda.h>
#include <stdio.h>

#define N 32

void printMatrix (unsigned* matrix) {
	for (unsigned i = 0; i < N * N; i++) {
		printf(" %u ", matrix[i]);
		if (i % N == (N-1)) {
			printf("\n");
		}
	}
} 

void createMatrix(unsigned* matrix) {
	for (unsigned i = 0; i < N; i++) {
		for (unsigned j = 0; j < N; j++) {
			if (i == j) {
				matrix[i * N + j] = i + 1;
			} else {
				matrix[i * N + j] = 0;
			}
		}
	}	
}

__global__ void square (unsigned* matrix, unsigned* result, unsigned matrixSize) {
	unsigned id = blockIdx.x * blockDim.x + threadIdx.x;
	for (unsigned jj = 0; jj < matrixSize; jj++) {
		for (unsigned kk = 0; kk < matrixSize; kk++) {
			result[id * matrixSize + jj] += matrix[id * matrixSize + kk] * matrix[kk * matrixSize + jj];
		}
	}
}

__host__ void call_sqr (unsigned* h_in_matrix, unsigned* h_out_matrix) {
//	unsigned n = N;
	unsigned *d_in_matrix, *d_out_matrix;
	cudaMalloc((void **) &d_in_matrix, N * N * sizeof(unsigned));
	cudaMalloc((void **) &d_out_matrix, N * N * sizeof(unsigned));
	cudaMemcpy(d_in_matrix, h_in_matrix, N * N * sizeof(unsigned), cudaMemcpyHostToDevice);
	
	square<<<1, N>>>(d_in_matrix, d_out_matrix, N);
	
	cudaMemcpy(h_out_matrix, d_out_matrix, N * N * sizeof(unsigned), cudaMemcpyDeviceToHost);

	cudaFree(d_in_matrix);
	cudaFree(d_out_matrix);	
}

int main() {
	unsigned* matrix;
	unsigned* result;
	matrix = (unsigned*)malloc(N * N * sizeof(unsigned));
	result = (unsigned*)malloc(N * N * sizeof(unsigned));
	createMatrix(matrix);
	call_sqr (matrix, result);
	printMatrix(result);
	free(matrix);
	free(result);

	return 0;
}
