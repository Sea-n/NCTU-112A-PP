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

    double x, y, r2;
    MPI_Status stats[96];
    long long rcv[96], cnt = 0;
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

    MPI_Gather(&cnt, 1, MPI_LONG_LONG, rcv, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        for (int k=1; k<world_size; k++)
            cnt += rcv[k];
    }

    if (world_rank == 0) {
        pi_result = 4.0 * cnt / N;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
