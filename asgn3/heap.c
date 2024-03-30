#include <stdio.h>
#include <stdbool.h>
#include "heap.h"
//#include "stats.c"
//#include <math.h>
//#define N 15

int max_child(Stats *stats,uint32_t *A, int first, int last)
{
	int left = first * 2;
	int right = left+ 1;

	if ((right <= last)&&(cmp(stats,A[right - 1], A[left - 1])>0))
	{
			return right;
	}
	return left;
}

void fix_heap(Stats *stats, uint32_t *A, int first, int last)
{
	bool found = false;
	int mother = first;
	int great = max_child(stats, A, mother, last);

	while ((mother <= (last / 2 ))&& !(found)) 
	{
		//if (A[mother - 1] < A[great -1])
		if(cmp(stats,A[great -1], A[mother - 1])>0)
		{
			swap(stats, &A[mother - 1], &A[great - 1]);
			mother = great;
			great = max_child(stats,A, mother, last);
		} else {
			found = true;
		}
	}
}

void build_heap(Stats *stats, uint32_t *A, int first, int len){
	int father;
	for (father = len/2; father >(first-1); father--){
		fix_heap(stats, A, father, len);
	}
}

void heap_sort(Stats *stats, uint32_t *A, uint32_t n)
{
	uint32_t first = 1;
	uint32_t leaf;
	reset(stats);
	build_heap(stats, A, first, n);
	for (leaf = n; leaf>first; leaf--)
	{
		swap(stats, &A[first - 1], &A[leaf - 1]);
		fix_heap(stats, A, first, leaf - 1);
	}
}

/*
int main(void){
	uint32_t givenArr[] = {9988,21224,434,6867,8790,1180,20,41,260,15,32343,565754,4345,6553,45632,567};//{13, 5, 8, 0, 1};
	Stats Lstats;
	heap_sort(&Lstats, givenArr, N);
	for (int i = 0; i < N; i++){
		printf("%d ", givenArr[i]);
	}
		printf("\n%lu,%lu\n",Lstats.moves,Lstats.compares);
}*/
