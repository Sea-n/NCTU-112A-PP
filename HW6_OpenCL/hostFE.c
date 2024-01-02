#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program) {
    cl_int status;
    int filterSize = filterWidth * filterWidth;

	cl_command_queue command_queue = clCreateCommandQueue(*context, *device, 0, &status);
	CHECK(status, "clCreateCommandQueue");


	/* Create CL memery with host pointer */
	cl_mem input_img_mem = clCreateBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			imageHeight * imageWidth * sizeof(float), inputImage, &status);

	cl_mem filter_mem = clCreateBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			imageHeight * imageWidth * sizeof(float), filter, &status);

	cl_mem output_img_mem = clCreateBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			imageHeight * imageWidth * sizeof(float), outputImage, &status);


	cl_kernel kernel = clCreateKernel(*program, "convolution", status);
	CHECK(status, "clCreateKernel");

	clSetKernelArg(kernel, 0, sizeof(cl_int), (void *) &filterWidth);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &filter_mem);
	clSetKernelArg(kernel, 2, sizeof(cl_int), (void *) &imageHeight);
	clSetKernelArg(kernel, 3, sizeof(cl_int), (void *) &imageWidth);
	clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *) &input_img_mem);
	clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *) &output_img_mem);

	size_t global_size[2] = { imageHeight, imageWidth };
	size_t local_size[2] = { 20, 20 };
	status = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL,
			global_size, local_size, 0, NULL, NULL);
	CHECK(status, "clEnqueueNDRangeKernel");

	clEnqueueMapBuffer(command_queue, output_img_mem, CL_TRUE, CL_MAP_READ,
			0, imageHeight * imageWidth * sizeof(float), 0, 0, 0, &status);
	CHECK(status, "clEnqueueMapBuffer");

	// cleanup
	clReleaseKernel(kernel);
	clReleaseMemObject(input_img_mem);
	clReleaseMemObject(filter_mem);
	clReleaseMemObject(output_img_mem);
	clReleaseCommandQueue(command_queue);
	return 0;
}
