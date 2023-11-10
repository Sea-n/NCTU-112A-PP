#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
	int numThreads, threadId;
	float x0, x1, y0, y1;
	int width, height;
	int maxIter;
	int *output;
} WorkerArgs;

static inline int mandel(float c_re, float c_im, int count) {
	float z_re = c_re, z_im = c_im;
	int k;
	for (k = 0; k < count; ++k) {
		if (z_re * z_re + z_im * z_im > 4.f)
			break;

		float new_re = z_re * z_re - z_im * z_im;
		float new_im = 2.f * z_re * z_im;
		z_re = c_re + new_re;
		z_im = c_im + new_im;
	}

	return k;
}

void workerThreadStart(WorkerArgs *const args) {
	const float dx = (args->x1 - args->x0) / args->width;
	const float dy = (args->y1 - args->y0) / args->height;

	for (int j = args->threadId; j < args->height; j += args->numThreads) {
		const float y = args->y0 + j * dy;
		for (int i = 0; i < (int) args->width; ++i) {
			const float x = args->x0 + i * dx;

			const int index = (j * args->width + i);
			args->output[index] = mandel(x, y, args->maxIter);
		}
	}
}

void mandelbrotThread(int numThreads, float x0, float y0, float x1, float y1,
		int width, int height, int maxIter, int output[]) {
	static constexpr int MAX_THREADS = 32;

	if (numThreads > MAX_THREADS) {
		fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
		exit(1);
	}

	std::thread workers[MAX_THREADS];
	WorkerArgs args[MAX_THREADS];

	for (int k=0; k<numThreads; k++) {
		args[k].x0 = x0; args[k].y0 = y0;
		args[k].x1 = x1; args[k].y1 = y1;
		args[k].width = width;
		args[k].height = height;
		args[k].maxIter = maxIter;
		args[k].numThreads = numThreads;
		args[k].output = output;
		args[k].threadId = k;
	}

	/* Open N-1 new threads */
	for (int k=1; k<numThreads; k++)
		workers[k] = std::thread(workerThreadStart, &args[k]);
	workerThreadStart(&args[0]);

	for (int k = 1; k < numThreads; k++)
		workers[k].join();
}
