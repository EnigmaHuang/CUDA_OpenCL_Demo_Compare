#include <CL/cl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "OpenCL_tools.h"

// Read kernel file into a string
int readCLKernelFile(const char *file_name, size_t *file_size, char **file_content) 
{
	// Open the file
	FILE *inf = fopen(file_name, "r");
	if (inf == NULL) 
	{
		printf("-- Error opening file %s\n", file_name);
		file_content = NULL;
		file_size = 0;
		return 1;
	}
	
	// Get its size
	fseek(inf, 0, SEEK_END);
	long size = ftell(inf);
	*file_size = (size_t)(size + 1);
	rewind(inf);

	// Read the kernel code as a string
	char *_content = (char*) malloc((size + 1) * sizeof(char));
	size_t read_source_size = fread(_content, 1, size * sizeof(char), inf);
	assert(*file_size == read_source_size + 1);
	_content[size] = '\0';
	*file_content = _content;
	
	fclose(inf);
	return 0;
}


// Get platform from platform lists
int getCLPlatform(cl_platform_id *platform, int platform_id)
{
	cl_uint numPlatforms; 
	cl_int  status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		printf("Error: Getting platforms! \n");
		*platform = NULL;
		return status;
	}
	
	if (numPlatforms > 0)
	{
		cl_platform_id *platforms = (cl_platform_id *) malloc(numPlatforms * sizeof(cl_platform_id));
		status	= clGetPlatformIDs(numPlatforms, platforms, NULL);
		if (platform_id < numPlatforms)
		{
			*platform = platforms[platform_id];
		} else {
			printf("[WARNING] Cannot find platform with id = %d (max %d), fallback to platform 0.\n", platform_id, numPlatforms - 1);
			*platform = platforms[0];
		}
		free(platforms);
		return status;
	} else {
		*platform = NULL;
		return -1;
	}
}

// Query the platform and choose all GPU devices
int getCLGPUDevicesID(cl_platform_id *platform, cl_device_id **device)
{
	cl_uint	  numDevices = 0;
	cl_device_id *_device   = NULL;
	cl_int	   status	 = clGetDeviceIDs(*platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	if (numDevices > 0) 
	{
		_device = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
		status  = clGetDeviceIDs(*platform, CL_DEVICE_TYPE_GPU, numDevices, _device, NULL);
		*device = _device;
		return status;
	} else {
		*device = NULL;
		return -1;
	}
}
