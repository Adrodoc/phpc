#include <stdlib.h>
#include <stdio.h>

double** matrix_new(int rows, int cols);
void matrix_delete(double** matrix);

int main() {
	double** my_matrix = matrix_new(10, 120);
	my_matrix[4][7] = 5.0;
	printf("%f", my_matrix[4][7]);
	matrix_delete(my_matrix);
}

double** matrix_new(int rows, int cols) {
	double** matrix = (double**)malloc(rows * sizeof(double*));
	for (int i = 0; i < rows; i++) {
		matrix[i] = (double*)malloc(cols * sizeof(double));
	}
	return matrix;
}

void matrix_delete(double** matrix) {
	int rows = 10;
	for (int i = 0; i < rows; i++) {
		free(matrix[i]);
	}
	free(matrix);
}
