#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

__device__ int mandel(float c_re, float c_im, int count) {
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < count; ++i) {
    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}

__global__ void mandelKernel(int* img_dev, float x0, float dx, int resX,
		float y0, float dy, int resY, int maxIterations) {
    for (int i=0; i<4; i++) {
	    int thisX = blockIdx.x * blockDim.x + threadIdx.x;
	    int thisY = (blockIdx.y * blockDim.y + threadIdx.y) * 4 + i;

	    float x = x0 + thisX * dx;
	    float y = y0 + thisY * dy;

	    int idx = resX * thisY + thisX;
	    img_dev[idx] = mandel(x, y, maxIterations);
    }
}

void hostFE(float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations) {
    float dx = (upperX - lowerX) / resX;
    float dy = (upperY - lowerY) / resY;

    int size = resX * resY * sizeof(int);
    int *img_host = (int *) malloc(size * sizeof(int));
    if (!img_host) {
        free(img_host);
        return;
    }
    cudaHostAlloc(&img_host, size, cudaHostAllocMapped);

    int *img_dev;
    size_t pitch;
    cudaMallocPitch(&img_dev, &pitch, resX * sizeof(int), resY);

    int block_x = 32;
    int block_y = 32;
    int grid_x = (int) ceil(resX / (float) block_x);
    int grid_y = (int) ceil(resY / (float) block_y / 4);

    dim3 dimBlock(block_x, block_y);
    dim3 dimGrid(grid_x, grid_y);
    mandelKernel<<<dimGrid, dimBlock>>>(img_dev, lowerX, dx, resX, lowerY, dy, resY, maxIterations);

    cudaDeviceSynchronize();

    cudaMemcpy(img_host, img_dev, size, cudaMemcpyDeviceToHost);
    memcpy(img, img_host, size);
    cudaFreeHost(img_host);
    cudaFree(img_dev);
}
