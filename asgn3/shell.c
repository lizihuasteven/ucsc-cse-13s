#include <stdio.h>
#include "shell.h"
//#include "stats.h"
#include "gaps.h"


//#include "stats.c"

//#define N 15



void shell_sort(Stats *stats, uint32_t *arr, uint32_t length)
{
	//uint32_t GAIE[length];
	//GAIE[length]={0};
	int j,t;
	uint32_t i;
	int gap,temp;
	stats->moves = stats->compares = 0;

	for(t=0;t<GAPS;t++)
	{ if(gaps[t]<(length+1)/2)break;
	}

	for(;t<GAPS;t++)
	{	gap = gaps[t];
		for (i = gap; i<length; i++) 
		{ temp= arr[i];
		  for(j=i;j>=gap;j-=gap)
		  { 
		    //if(arr[j-gap]<temp) break;
			//arr[j]=arr[j-gap];
			if(cmp(stats, arr[j-gap],temp)<0) break;
			arr[j]=move(stats,arr[j-gap]);
			
		  }
		  //arr[j]=temp;
		  arr[j]=move(stats,temp);
		}
	}	
}

/*
int main(void) {
    printf("Hello World111\n");
    uint32_t  larr[N] = {9988,21224,434,6867,8790,1180,20,41,260,15,32343,565754,4345,6553,45632,567};
    int i;
	Stats Lstats;
	//reset(&Lstats);

	shell_sort(&Lstats,larr,N);
	for(i=0;i<N;i++)
    printf("%d,",larr[i]);

	printf("\n%lu,%lu\n",Lstats.moves,Lstats.compares);
    return 0;
}*/
