#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

__global__ void cuda_vector_add(int *a, int *b)
{
	__shared__ int results[64];  // Actually we don't need this, just for illustration
	
	int global_thread_id = blockIdx.x * blockDim.x + threadIdx.x;
	int local_thread_id  = threadIdx.x;
	
	results[local_thread_id] = a[global_thread_id] + b[global_thread_id];
	
	__syncthreads();
	
	a[global_thread_id] = results[local_thread_id];
}

int main(int argc, char **argv)
{
	int n, nBytes;
	n = atoi(argv[1]);
	n = (n + 63) / 64 * 64;
	nBytes = sizeof(int) * n;
	printf("Vector add, length = %d\n", n);
	
	int *h_a, *h_b, *d_a, *d_b;
	// Allocate memory on host
	h_a = (int*) malloc(nBytes);
	h_b = (int*) malloc(nBytes);
	// Allocate memory on device
	cudaMalloc((void**) &d_a, nBytes);
	cudaMalloc((void**) &d_b, nBytes);
	
	// Init data on host
	for (int i = 0; i < n; i++)
	{
		h_a[i] = 114 + i;
		h_b[i] = 514 - i;
	}
	
	// Copy data to device
	cudaMemcpy(d_a, h_a, nBytes, cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, h_b, nBytes, cudaMemcpyHostToDevice);
	
	// Set kernel arguments and launch kernel
	dim3 block(64);
	dim3 grid(n / block.x);
	cuda_vector_add<<<grid, block>>>(d_a, d_b);
	
	// Generate result on host
	for (int i = 0; i < n; i++)	h_b[i] += h_a[i];
	
	// Copy result from device to host
	cudaMemcpy(h_a, d_a, nBytes, cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize();
	
	// Check the results
	for (int i = 0; i < n; i++) assert(h_a[i] == h_b[i]);
	printf("Result is correct.\n");
	
	// Free host memory
	free(h_a);
	free(h_b);
	
	// Free device memory
	cudaFree(d_a);
	cudaFree(d_b);
	
	return 0;
}