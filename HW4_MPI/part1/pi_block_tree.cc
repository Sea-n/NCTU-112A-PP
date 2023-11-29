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

    MPI_Status stat;
    double x, y, r2;
    long long rcv, cnt = 0;
    unsigned int seed1 = world_rank * 2;
    unsigned int seed2 = seed1 + 1;

    for (int toss = N / world_size; toss > 0; toss--) {
        x = rand_r(&seed1);
        y = rand_r(&seed2);
        r2 = x * x + y * y;
        if (r2 <= m2)
            cnt++;
    }

    for (int b=1; b<=world_size; b<<=1) {
        if (world_rank & (b-1))
            break;
        if (world_rank & b) {
            MPI_Send(&cnt, 1, MPI_LONG_LONG, world_rank ^ b, 0, MPI_COMM_WORLD);
        } else if ((world_rank ^ b) < world_size) {
            MPI_Recv(&rcv, 1, MPI_LONG_LONG, world_rank ^ b, 0, MPI_COMM_WORLD, &stat);
            cnt += rcv;
        }
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
