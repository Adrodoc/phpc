#include <stdlib.h>
#include <stdio.h>

double** matrix_new(int rows, int cols);
void matrix_delete(double** matrix);

int main() {
	int rows = 5;
	int cols = 8;
	double** my_matrix = matrix_new(rows, cols);
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			my_matrix[r][c] = r * cols + c;
		}
	}
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			printf("%f\t", my_matrix[r][c]);
		}
		printf("\n");
	}
	matrix_delete(my_matrix);
}

double** matrix_new(int rows, int cols) {
	double** matrix = (double**)malloc(rows * sizeof(double*));
	double* rowPointer = (double*)malloc(rows * cols * sizeof(double));
	for (int i = 0; i < rows; i++) {
		matrix[i] = &rowPointer[i * cols];
	}
	return matrix;
}

void matrix_delete(double** matrix) {
	free(*matrix);
	free(matrix);
}
