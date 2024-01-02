__kernel void convolution(const int filterWidth, __global float *filter,
                          const int imageHeight, const int imageWidth,
                          __global float *inputImage, __global float *outputImage) {
    int hsz = filterWidth / 2;
    int x = get_global_id(0);
    int y = get_global_id(1);
    float sum = 0;
    int i, j;

    for (i=-hsz; i<=hsz; i++)
        for (j=-hsz; j<=hsz; j++)
            if (x+i >= 0 && x+i < imageHeight &&
                y+j >= 0 && y+j < imageWidth) {
                sum += inputImage[(x+i) * imageWidth + (y+j)] *
                       filter[(i+hsz) * filterWidth + (j+hsz)];
            }
    outputImage[x * imageWidth + y] = sum;
}
