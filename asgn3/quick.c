#include <stdio.h>
#include "quick.h"
//#include "stats.c"
//#define N 6

int partition (Stats *stats, uint32_t *A, int low, int high) {
	int i = low - 1;
	for (; low < high; low++) {
		if (cmp(stats, A[low - 1], A[high - 1]) < 0) {
			i += 1;
			swap(stats, &A[i - 1], &A[low - 1]);
		}
	}
	swap(stats, &A[i], &A[high - 1]);
	return (i + 1);
}

void quick_sorter(Stats *stats, uint32_t *A, int low, int high) {
	if (low < high) {
		int p = partition(stats, A, low, high);
		quick_sorter(stats, A, low, p - 1);
		quick_sorter(stats, A, p + 1, high);
	}
}

void quick_sort(Stats *stats, uint32_t *A, uint32_t n) {
	quick_sorter(stats, A, 1, n);
}

/*
int main(void) {
	uint32_t arr[] = {11, 11, 51, 9, 8, 2};
	Stats Lstats;
	reset(&Lstats);
	quick_sort(&Lstats, arr, N);
	for (int i = 0; i < N; i++){
		printf("%d ", arr[i]);
	}
		printf("\n%lu,%lu\n",Lstats.moves,Lstats.compares);
}
*/
