#ifndef _OPENCL_TOOLS_H_
#define _OPENCL_TOOLS_H_

#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

// Read kernel file into a string
int readCLKernelFile(const char *file_name, size_t *file_size, char **file_content);

// Get platform from platform lists
int getCLPlatform(cl_platform_id *platform, int platform_id);

// Query the platform and choose all GPU devices
int getCLGPUDevicesID(cl_platform_id *platform, cl_device_id **device);

#endif
