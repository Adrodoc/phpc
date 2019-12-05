#define SUPERMUC 1
//#define PAPI 1
#define __GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef PAPI
#include <papi.h>
#endif // PAPI

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

#define BLOCK_SIZE_M 40
#define BLOCK_SIZE_N 40
#define BLOCK_SIZE_K 40

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define long_long_t long long

static inline long_long_t timestamp();

//
// C = A*B
// (M x N) * (N x K) -> (M x K)
//
void mmult(double A[SIZE_M][SIZE_N],
	double B[SIZE_K][SIZE_N],
	double C[SIZE_M][SIZE_K])
{
	int    m, k, n;
	for (m = 0; m < SIZE_M; m += BLOCK_SIZE_M) {
		int m2max = BLOCK_SIZE_M + m;
		for (k = 0; k < SIZE_K; k += BLOCK_SIZE_K) {
			int k2max = BLOCK_SIZE_K + k;
			for (n = 0; n < SIZE_N; n += BLOCK_SIZE_N) {
				int n2max = BLOCK_SIZE_N + n;
				for (int m2 = m; m2 < m2max; m2++) {
					for (int k2 = k; k2 < k2max; k2++) {
						double sum = 0.0;
						for (int n2 = n; n2 < n2max; n2++) {
							sum += A[m2][n2] * B[k2][n2];
						}
						if (n == 0)
							C[m2][k2] = sum;
						else
							C[m2][k2] += sum;
					}
				}
			}
		}
	}
}

void mmult_orig(double A[SIZE_M][SIZE_N],
	double B[SIZE_K][SIZE_N],
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
	double B[SIZE_K][SIZE_N],
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

int main(int argc, char* argv[])
{
	int         i, j;
	double      sum;
	/* Using double for nflop to prevent overflow on 32 bit archs */
	double      nflop;
	long_long_t tstart, tstop;
	double      tmmult;

	printf("Problem size: %d x %d\n", SIZE, SIZE);

	double(*A)[SIZE_M];
	double(*B)[SIZE_K];
	double(*C)[SIZE_M];
	size_t alignment = 64;
#ifdef SUPERMUC
	posix_memalign((void**)&A, alignment, (SIZE_M) * (SIZE_N) * sizeof(double));
	posix_memalign((void**)&B, alignment, (SIZE_K) * (SIZE_N) * sizeof(double));
	posix_memalign((void**)&C, alignment, (SIZE_M) * (SIZE_K) * sizeof(double));
#else
	A = (double(*)[SIZE_M]) malloc(SIZE_M * SIZE_N * sizeof(double));
	B = (double(*)[SIZE_K]) malloc(SIZE_K * SIZE_N * sizeof(double));
	C = (double(*)[SIZE_M]) malloc(SIZE_M * SIZE_K * sizeof(double));
#endif // SUPERMUC

	for (i = 0; i < SIZE_M; i++) {
		for (j = 0; j < SIZE_N; j++) {
			A[i][j] = (double)i + (double)j;
		}
	}

	for (i = 0; i < SIZE_N; i++) {
		for (j = 0; j < SIZE_K; j++) {
			B[i][j] = (double)(i)+(double)(j);
		}
	}

	/* Two FLOP in inner loop: add and mul */
	nflop = 2.0 * (double)SIZE_M * (double)SIZE_N * (double)SIZE_K * 10;

#ifdef PAPI
	int events[3] = { PAPI_L1_DCM, PAPI_L2_DCM, PAPI_L3_DCM };
	int numEvents = 3;
	if (PAPI_start_counters(events, numEvents) != PAPI_OK) {
		printf("kaputt\n");
		return 1;
	}
#endif // PAPI
#ifdef SUPERMUC
	tstart = timestamp();
#endif // SUPERMUC

	for (int i = 0; i < 10; i++) {
		mmult(A, B, C);
	}

#ifdef SUPERMUC
	tstop = timestamp();
#endif // SUPERMUC
#ifdef PAPI
	long long values[3];
	if (PAPI_stop_counters(values, numEvents) != PAPI_OK) {
		printf("kaputt\n");
		return 1;
	}
	printf("PAPI_L1_DCM=%lld", values[0]);
	printf("PAPI_L2_DCM=%lld", values[0]);
	printf("PAPI_L3_DCM=%lld", values[0]);
#endif // PAPI

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

	free(A);
	free(B);
	free(C);

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
