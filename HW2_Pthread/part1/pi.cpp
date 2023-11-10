#include <pthread.h>
#include <cstdio>
#include <random>

constexpr double m2 = (double) RAND_MAX * RAND_MAX;

typedef struct  {
	int tid, begin, end;
	long long *sum;
} Job;

pthread_t thrd[64];
pthread_mutex_t mtx;

void *worker(void *arg);

int main(int argc, char **argv) {
	long long T, N;
	double pi;
	long long *sum = (long long *) malloc(sizeof(long long));
	void *status;

	/* pthread init */
	pthread_mutex_init(&mtx, NULL);
	pthread_attr_t pattr;
	pthread_attr_init(&pattr);
	pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);

	*sum = 0;

	/* Command line arguments */
	T = strtol(argv[1], nullptr, 10);
	N = strtoll(argv[2], nullptr, 10);

	/* Setup jobs and start worker */
	Job jobs[T];
	for (int k=0; k<T; k++) {
		jobs[k].tid = k;
		jobs[k].begin = N * k / T;
		jobs[k].end = N * (k+1) / T;
		jobs[k].sum = sum;  // shared pointer
		pthread_create(&thrd[k], &pattr, worker, (void *) &jobs[k]);
	}

	/* Wrapping up */
	pthread_attr_destroy(&pattr);
	for (int k=0; k<T; k++) {
		pthread_join(thrd[k], &status);
	}
	pthread_mutex_destroy(&mtx);

	pi = 4.0 * (*sum) / N;
	printf("%lf\n", pi);

	pthread_exit(NULL);
	return 0;
}

void *worker(void *arg) {
	Job *job = (Job *) arg;
	double x, y, r2, pi;
	long long cnt = 0;
	unsigned int seed1 = job->tid * 2;
	unsigned int seed2 = seed1 + 1;

	for (int toss = job->begin; toss < job->end; toss++) {
		x = rand_r(&seed1);
		y = rand_r(&seed2);
		r2 = x * x + y * y;
		if (r2 <= m2)
			cnt++;
	}

	pthread_mutex_lock(&mtx);
	*(job->sum) += cnt;
	pthread_mutex_unlock(&mtx);

	pthread_exit((void *)0);
}
