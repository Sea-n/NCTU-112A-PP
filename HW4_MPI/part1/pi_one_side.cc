#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstdio>
#include <random>
#include <time.h>
#include <mpi.h>

constexpr double m2 = (double) RAND_MAX * RAND_MAX;

int main(int argc, char **argv) {
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    int N = atoi(argv[1]);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Win win;
    double x, y, r2;
    MPI_Status stats[96];
    long long sum = 0, cnt = 0;
    MPI_Request requests[world_size];
    unsigned int seed1 = world_rank * 2;
    unsigned int seed2 = seed1 + 1;

    for (int toss = N / world_size; toss > 0; toss--) {
        x = rand_r(&seed1);
        y = rand_r(&seed2);
        r2 = x * x + y * y;
        if (r2 <= m2)
            cnt++;
    }

    if (world_rank == 0) {
        MPI_Win_create(&sum, sizeof(long long), sizeof(long long), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    } else {
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    MPI_Win_fence(0, win);
    MPI_Accumulate(&cnt, 1, MPI_LONG_LONG, 0, 0, 1, MPI_LONG_LONG, MPI_SUM, win);
    MPI_Win_fence(0, win);

    if (world_rank == 0) {
        pi_result = 4.0 * sum / N;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}
