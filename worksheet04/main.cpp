#define __GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef SIZE
#define SIZE 1000
#endif

#ifndef SIZE_M
#define SIZE_M SIZE
#endif

#ifndef SIZE_N
#define SIZE_N SIZE
#endif

#ifndef SIZE_K
#define SIZE_K SIZE
#endif

#define BLOCK_SIZE 40

#define SUPERMUC 1

#define long_long_t long long

static inline long_long_t timestamp();

//
// C = A*B
// (M x N) * (N x K) -> (M x K)
//
void mmult(double A[SIZE_M][SIZE_N],
	double B[SIZE_N][SIZE_K],
	double C[SIZE_M][SIZE_K])
{
	int    m, k, n;
	for (m = 0; m < SIZE_M; m += BLOCK_SIZE) {
		int m2max = BLOCK_SIZE + m;
		for (n = 0; n < SIZE_N; n += BLOCK_SIZE) {
			int n2max = BLOCK_SIZE + n;
			for (k = 0; k < SIZE_K; k += BLOCK_SIZE) {
				int k2max = BLOCK_SIZE + k;
				for (int m2 = m; m2 < m2max; m2++) {
					for (int n2 = n; n2 < n2max; n2++) {
						for (int k2 = k; k2 < k2max; k2++) {

							if (n2 == 0)
								C[m2][k2] = A[m2][n2] * B[n2][k2];
							else
								C[m2][k2] += A[m2][n2] * B[n2][k2];

						}

					}

				}

			}
		}
	}
}

void mmult_orig(double A[SIZE_M][SIZE_N],
	double B[SIZE_N][SIZE_K],
	double C[SIZE_M][SIZE_K])
{
	int    i, j, k;
	double sum;
	for (i = 0; i < SIZE_M; i++) {
		for (j = 0; j < SIZE_K; j++) {
			sum = 0.0;
			for (k = 0; k < SIZE_N; k++) {
				sum += A[i][k] * B[k][j];
			}
			C[i][j] = sum;
		}
	}
}

void validate(double A[SIZE_M][SIZE_N],
	double B[SIZE_N][SIZE_K],
	double C[SIZE_M][SIZE_K]
) {
	double D[SIZE_M][SIZE_K];
	mmult_orig(A, B, D);
	for (int i = 0; i < SIZE_M; i++) {
		for (int j = 0; j < SIZE_K; j++) {
			if (C[i][j] != D[i][j]) {
				printf("C[%d][%d] != D[%d][%d]\n", i, j, i, j);
			}
		}
	}
}

double A[SIZE_M][SIZE_N];
double B[SIZE_N][SIZE_K];
double C[SIZE_M][SIZE_K];

int main(int argc, char* argv[])
{
	int         i, j;
	double      sum;
	/* Using double for nflop to prevent overflow on 32 bit archs */
	double      nflop;
	long_long_t tstart, tstop;
	double      tmmult;

	printf("Problem size: %d x %d\n", SIZE, SIZE);

	for (i = 0; i < SIZE_M; i++) {
		for (j = 0; j < SIZE_N; j++) {
			A[i][j] = (double)(i)+(double)(j);
		}
	}

	for (i = 0; i < SIZE_N; i++) {
		for (j = 0; j < SIZE_K; j++) {
			B[i][j] = (double)(i)+(double)(j);
		}
	}

	/* Two FLOP in inner loop: add and mul */
	nflop = 2.0 * (double)SIZE_M * (double)SIZE_N * (double)SIZE_K * 10;

#ifdef SUPERMUC
	tstart = timestamp();
#endif // SUPERMUC
	for (int i = 0; i < 10; i++) {
		mmult(A, B, C);
	}
#ifdef SUPERMUC
	tstop = timestamp();
#endif // SUPERMUC

	/* Duration in nanoseconds.
	 * FLOP/ns = GFLOP/s
	 */
#ifdef SUPERMUC
	tmmult = (double)(tstop - tstart);
#endif // SUPERMUC

	/* Sum matrix elements as correctness hint */
	validate(A, B, C);

	sum = 0.0;
	for (i = 0; i < SIZE_M && i < SIZE_K; i++) {
		sum += C[i][i];
	}
	printf("Trace mmult: %12.12g\n", sum);
	printf("M, N, K, tmmult_s, gflops_mmult\n");
#ifdef SUPERMUC
	printf("%d, %d, %d, %f, %f \n",
		SIZE_M, SIZE_N, SIZE_K,
		tmmult, nflop / tmmult);
#endif // SUPERMUC

	return 0;
}

#ifdef SUPERMUC
static inline long_long_t timestamp()
{
	struct timespec ts;
	long_long_t timestamp;
	clock_gettime(CLOCK_REALTIME, &ts);
	timestamp = ts.tv_sec * 1000000000LL + ts.tv_nsec;
	return timestamp;
}
#endif // SUPERMUC
