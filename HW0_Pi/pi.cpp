#include <cstdio>
#include <random>

int main() {
	int cnt = 0;
	const int N = 50000000;
	double x, y, r2, pi;

	srand(time(0));
	std::uniform_real_distribution<double> unif(0, 1);
	std::default_random_engine re;

	for (int toss = 0; toss < N; toss++) {
		x = unif(re);
		y = unif(re);
		r2 = x * x + y * y;
		if (r2 <= 1)
			cnt++;
	}
	pi = 4 * cnt / ((double) N);

	printf("%lf\n", pi);
	return 0;
}
