//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "OpenCL_tools.h"

int main(int argc, char **argv)
{
	int n = atoi(argv[1]);
	n = (n + 63) / 64 * 64;
	int nBytes = sizeof(int) * n;
	printf("Vector add, length = %d\n", n);
	
    // Allocate memory on host
	int *h_a, *h_b;
	h_a = (int*) malloc(nBytes);
	h_b = (int*) malloc(nBytes);
	
	// Init data on host
	for (int i = 0; i < n; i++)
	{
		h_a[i] = 114 + i;
		h_b[i] = 514 - i;
	}
	
    // OpenCL extra step 1: getting platforms and choose an available one(first)
	cl_int err;
    cl_platform_id platform;
    getCLPlatform(&platform, 0);

    // OpenCL extra step 2: query the platform
    cl_device_id *GPU_devices;
	getCLGPUDevicesID(&platform, &GPU_devices);

    // OpenCL extra step 3: create context
    cl_context context = clCreateContext(NULL, 1, GPU_devices, NULL, NULL, NULL);

    // OpenCL extra step 4: creating command queue associate with the context
	// GPU_devices[0] means we use the first GPU
    cl_command_queue queue = clCreateCommandQueue(context, GPU_devices[0], 0, NULL);

    // OpenCL extra step 5: create program object
    char   kernel_file_name[] = "vector_add.cl";
	char   *_source_content;
	size_t source_size;
	err = readCLKernelFile(kernel_file_name, &source_size, &_source_content);
	const char *source_content = _source_content;
    cl_program program = clCreateProgramWithSource(context, 1, &source_content, &source_size, NULL);

    // OpenCL extra step 6: build program
	// The 2nd and 3rd parameters mean that we use the first GPU 
    err = clBuildProgram(program, 1, GPU_devices, NULL, NULL, NULL);
	
	// OpenCL extra step 7: create kernel object
    cl_kernel kernel = clCreateKernel(program, "ocl_vector_add", NULL);

	// Allocate memory on device
	cl_mem d_a = clCreateBuffer(context, CL_MEM_READ_WRITE, nBytes, NULL, &err);
	cl_mem d_b = clCreateBuffer(context, CL_MEM_READ_WRITE, nBytes, NULL, &err);
	
	// Copy data to device
	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, nBytes, h_a, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, nBytes, h_b, 0, NULL, NULL);

    // Set kernel arguments and launch kernel
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &d_a);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &d_b);
	const size_t threads_in_workgroup[1] = {64};
    const size_t workspace_threads[1]    = {n};
    cl_event event;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, workspace_threads, threads_in_workgroup, 0, NULL, &event);
    clWaitForEvents(1, &event);
    clReleaseEvent(event);

	// Generate result on host
	for (int i = 0; i < n; i++)	h_b[i] += h_a[i];
	
    // Copy result from device to host
    err = clEnqueueReadBuffer(queue, d_a, CL_TRUE, 0, nBytes, h_a, 0, NULL, NULL);
	
	// Check the results
	for (int i = 0; i < n; i++) assert(h_a[i] == h_b[i]);
	printf("Result is correct.\n");

	// Free host memory
    free(h_a);
	free(h_b);
	free(_source_content);
    free(GPU_devices);
	
    // Free device resources
    err = clReleaseKernel(kernel);      // Release kernel
    err = clReleaseProgram(program);    // Release the program object
    err = clReleaseMemObject(d_a);      // Release mem object
    err = clReleaseMemObject(d_b);	    // Release mem object
    err = clReleaseCommandQueue(queue); // Release Command queue
    err = clReleaseContext(context);    // Release context
	
	return 0;
}