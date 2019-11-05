#include <stdlib.h>
#include <stdio.h>

int main() {
	int* array_d = (int*)malloc(100 * sizeof(int));
	int array_s[100];
	for (int i = 0; i < 100; i++) {
		array_d[i] = i;
	}
	for (int i = 0; i < 100; i++) {
		array_s[i] = array_d[99 - i];
	}
	printf("array_d:\n");
	for (int i = 0; i < 100; i++) {
		printf("%d\n", array_d[i]);
	}
	printf("array_s:\n");
	for (int i = 0; i < 100; i++) {
		printf("%d\n", array_s[i]);
	}
	free(array_d);
}
