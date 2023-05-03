#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <chrono>
#include <ctime>

__global__ void matrixMul(int* a, int* b, int* c, int n) {
	// Compute each thread's row
	int row = blockIdx.y * blockDim.y + threadIdx.y;
	// Compute each thread's column
	int col = blockIdx.x * blockDim.x + threadIdx.x;

	int temp_sum = 0;
	// Boundary protection
	if ((row < n) && (col < n)) {
		// Iterate over row, and down column
		for (int k = 0; k < n; k++) {
			// Accumulate result for a single element
			temp_sum += a[row * n + k] * b[k * n + col];
		}
		// Assign result
		c[row * n + col] = temp_sum;
	}
}

// Initialization function for matrices
void matrix_init(int* a, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			a[i * n + j] = rand() % 100;
		}
	}
}

// This program multiplies two squire matrix. Therefore, a single constant for matrix dimension is sufficient
int dim;  // Default value 4096
// the block size for blocked matrix-matrix multiplication
int blockSize; // Default value 16

int main(int argc, const char* argv[]) {
	// read the matrix dimension length and block size from command line
	if (argc > 2) {
		dim = atoi(argv[1]);
		blockSize = atoi(argv[2]);
	}
	else {
		// Assigning default param values here.
		dim = 4096;
		blockSize = 16;
		printf("Assigning default value dim = %d and blockSize = %d", dim, blockSize);
	}

	// Taking the start time
	auto start = std::chrono::system_clock::now();

	// Matrix size of 4096 x 4096;
	int n = dim;

	// Size (in bytes) of matrix
	size_t bytes = n * n * sizeof(int);

	// Host pointers
	int* h_a, * h_b, * h_c;

	// Allocate host memory
	h_a = (int*)malloc(bytes);
	h_b = (int*)malloc(bytes);
	h_c = (int*)malloc(bytes);

	// Device pointers
	int* d_a, * d_b, * d_c;

	// Allocated device memory
	cudaMalloc(&d_a, bytes);
	cudaMalloc(&d_b, bytes);
	cudaMalloc(&d_c, bytes);

	// Initialize matrices
	matrix_init(h_a, n);
	matrix_init(h_b, n);

	// Copy data to the device
	cudaMemcpy(d_a, h_a, bytes, cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, h_b, bytes, cudaMemcpyHostToDevice);

	// Threads per block
	int BLOCK_SIZE = 16;

	// Blocks in each dimension (No padding)
	int GRID_SIZE = n / BLOCK_SIZE;

	// Use dim3 objects
	dim3 grid(GRID_SIZE, GRID_SIZE);
	dim3 threads(BLOCK_SIZE, BLOCK_SIZE);

	// Launch kernel
	matrixMul << <grid, threads >> > (d_a, d_b, d_c, n);

	// Copy back to the host
	cudaMemcpy(h_c, d_c, bytes, cudaMemcpyDeviceToHost);

	// Free memory on host
	free(h_a);
	free(h_b);
	free(h_c);

	// Free memory on device
	cudaFree(d_a);
	cudaFree(d_b);
	cudaFree(d_c);

	printf("\nCalculated the matrix successfully...\n");

	// Getting the end time
	auto end = std::chrono::system_clock::now();

	// Calculating elapsed_seconds
	std::chrono::duration<double> elapsed_seconds = end - start;

	// Print elasped time in seconds
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;

	return 0;
}