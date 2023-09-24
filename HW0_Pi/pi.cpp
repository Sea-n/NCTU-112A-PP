#include <iostream>
#include <random>

using namespace std;

int main() {
	int number_in_circle = 0;
	int number_of_tosses = 100;
	double x, y, distance_squared;

	uniform_real_distribution<double> unif(-1, 1);
	default_random_engine re;

	for (int toss = 0; toss < number_of_tosses; toss++) {
		x = unif(re);
		y = unif(re);
		distance_squared = x * x + y * y;
		if (distance_squared <= 1)
			number_in_circle++;
	}
	double pi_estimate = 4 * number_in_circle / ((double) number_of_tosses);

	cout << pi_estimate;
	return 0;
}
