#include <stdio.h>
#include "batcher.h"
//#include "stats.h"
//#include "gaps.h"

//#include "stats.c"
/*
extern int cmp(Stats *stats, uint32_t x, uint32_t y);

extern uint32_t move(Stats *stats, uint32_t x);

extern void swap(Stats *stats, uint32_t *x, uint32_t *y);

extern  void reset(Stats *stats);*/

//#define N 15

void comparator(Stats *stats,uint32_t *A,uint32_t x,uint32_t y)
{
		if(cmp(stats, A[x],A[y])>0)
			swap(stats, &A[x], &A[y]);
}

void batcher_sort(Stats *stats, uint32_t *A, uint32_t n)
{
	uint32_t i,t,p,q,r,d;
  uint32_t sign,temp;
  reset(stats);
	//if len(A)==0:return
	if (n>0)
	{ 
		//n=len(A)
		//t= n.bit_length();
		temp = n;
		for(i=0,t=0,sign=0;i<32;i++)
		{ if(sign==0)
			{  if(temp&0x80000000)
				 { 	sign=1;
				 	 	t=1;
				 }
		  }
		  else t++;
		  temp <<= 1;
		}

		p=1<<(t-1);
		
		while(p>0)
		{ 
		  q= 1<<(t-1);
		  r =0;
		  d= p;
		  while(d>0)
		  {	for (i = 0; i<(n-d); i++) 
				{ if((i&p)==r)
					   comparator(stats,A,i,i+d);
				}
				d = q-p;
				q >>= 1;
				r = p;
			}
			p >>=1;
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

	batcher_sort(&Lstats,larr,N);
	for(i=0;i<N;i++)
    printf("%d,",larr[i]);

	printf("\n%lu,%lu\n",Lstats.moves,Lstats.compares);
    return 0;
}*/
