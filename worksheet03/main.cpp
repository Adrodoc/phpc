#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NITERS 1000000
#define TYPE double

int main() {
	unsigned int maxN = 1 << 15;
	TYPE* a = (TYPE*)malloc(maxN * sizeof(TYPE));
	TYPE* b = (TYPE*)malloc(maxN * sizeof(TYPE));
	TYPE* c = (TYPE*)malloc(maxN * sizeof(TYPE));
	TYPE* d = (TYPE*)malloc(maxN * sizeof(TYPE));

	printf("%d\n", CLOCKS_PER_SEC);

	for (size_t n = 1; n < maxN; n *= 2) {
		printf("Running benchmark for n=%d\n",n);
		clock_t start = clock();
		for (size_t j = 0; j < NITERS; j++) {
			for (size_t i = 0; i < n; i++) {
				a[i] = b[i] + c[i] * d[i];
			}
		}
		clock_t end = clock();
		clock_t clocks = end - start;
		double seconds = (double) clocks / CLOCKS_PER_SEC;
		double flops = (double)2 * n * NITERS / seconds / 1024 / 1024 / 1024;
		printf("took %d cycles (%f seconds) with %f gibiflops\n", clocks, seconds, flops);
	}
}
