#include <cuda.h>
#include <stdio.h>

__global__ void dkernel (unsigned* arr) {
	unsigned id = blockIdx.x * blockDim.x * blockDim.y * blockDim.z 
		    + threadIdx.z * blockDim.y * blockDim.x 
		    + threadIdx.y * blockDim.x 
		    + threadIdx.x;
	arr[id] = 0;
//	printf ("threadIdx. x, y, z = %d, %d, %d\n", threadIdx.x, threadIdx.y, threadIdx.z);
}

__global__ void add (unsigned* arr) {
	unsigned id = blockIdx.x * blockDim.x * blockDim.y * blockDim.z 
		    + threadIdx.z * blockDim.y * blockDim.x 
		    + threadIdx.y * blockDim.x 
		    + threadIdx.x;
	arr[id] += id;
}


#define N 16	// blockDim.x
#define M 8	// blockDim.y
#define L 8	// blockDim.z

int main() {
	dim3 block(N, M, L);
	unsigned *arr, *harr;
	printf ("Size of Array = %d\n", N * M * L);
	cudaMalloc (&arr, N * M * L * sizeof(unsigned));
	harr = (unsigned *)malloc (N * M * L * sizeof(unsigned));
	dkernel<<<1, block>>> (arr);
	add<<<1, block>>> (arr);
	cudaMemcpy (harr, arr, N * M * L * sizeof(unsigned), cudaMemcpyDeviceToHost);
	for (unsigned ii = 0; ii < N; ii++) {
		for (unsigned jj = 0; jj < M; jj++) {
			for (unsigned kk = 0; kk < L; kk++) {
				printf ("%d ", harr[ii * M * L + jj * L + kk]);
			}
			printf("\n");
		}
		printf("\n");
	}
	return 0;
}
