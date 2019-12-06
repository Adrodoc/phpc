#include <math.h>
#include <omp.h>
#include <stdio.h>
#define M_PI 3.14159265358979323846

int main() {
	omp_set_num_threads(5);
	// exercise1();
	exercise2();
}

int exercise1() {
	// a)
	printf("Unordered:\n");
#pragma	omp parallel
	{
		int thread_num = omp_get_thread_num();
		int num_threads = omp_get_num_threads();
		printf("Hello, I'm thread number %d of %d threads\n", thread_num, num_threads);
	}

	// b
	printf("Ordered:\n");
	int i;
#pragma omp parallel for ordered
	for (i = 0; i < omp_get_num_threads(); i++) {
		int thread_num = omp_get_thread_num();
		int num_threads = omp_get_num_threads();
#pragma omp ordered
		printf("Hello, I'm thread number %d of %d threads\n", thread_num, num_threads);
	}
}

int exercise2() {
	int niter = 1000000;
	// a)
	double pi_forth = 0;
	for (int i = 0; i < niter; i++) {
		double divisor = i * 2 + 1;
		if (i % 2 == 0) {
			pi_forth += 1 / divisor;
		}
		else {
			pi_forth -= 1 / divisor;
		}
	}
	printf("Calculated PI: %f\n", pi_forth * 4);
	printf("Actual PI: %f\n", M_PI);
	printf("Diff: %f\n", fabs(M_PI - pi_forth * 4));

	// b)
}
