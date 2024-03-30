#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include "shell.c"
#include "batcher.c"
#include "heap.c"
#include "quick.c"
#include "stats.c"
#define OPTIONS "Hasbhqn:p:r:"

int main(int argc, char **argv) {
	int select_a = 0, select_s = 0, select_b = 0, select_h = 0, select_q = 0, select_n = 0, select_p = 0, select_r = 0, n_arg = 100, p_arg = 100, r_arg = 13371453;
	int opt = 0;
	uint32_t *Array = (uint32_t *)calloc(10, sizeof(uint32_t));
	// nums, this is for counting how many elements have been printed, in order to switch to the next line
	int nums = 0;
	Stats Lstats;
	//if no parameters are detected, print default msg and end program
	if (argc == 1) {
		printf("Select at least one sort to perform.\nSYNOPSIS\n   A collection of comparison-based sorting algorithms.\n\nUSAGE\n   ./sorting [-Hasbhqn:p:r:] [-n length] [-p elements] [-r seed]\n\nOPTIONS\n   -H              Display program help and usage.\n   -a              Enable all sorts.\n   -s              Enable Shell Sort.\n   -b              Enable Batcher Sort.\n   -h              Enable Heap Sort.\n   -q              Enable Quick Sort.\n   -n length       Specify number of array elements (default: 100).\n   -p elements     Specify number of elements to print (default: 100).\n   -r seed         Specify random seed (default: 13371453).\n");
		return 0;
	}
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		switch (opt) {
			case 'H':
				printf("SYNOPSIS\n   A collection of comparison-based sorting algorithms.\n\nUSAGE\n   ./sorting [-Hasbhqn:p:r:] [-n length] [-p elements] [-r seed]\n\nOPTIONS\n   -H              Display program help and usage.\n   -a              Enable all sorts.\n   -s              Enable Shell Sort.\n   -b              Enable Batcher Sort.\n   -h              Enable Heap Sort.\n   -q              Enable Quick Sort.\n   -n length       Specify number of array elements (default: 100).\n   -p elements     Specify number of elements to print (default: 100).\n   -r seed         Specify random seed (default: 13371453).\n");
				return 0;
			case 'a':
				//enable all
				select_a = 1;
				break;
			case 's':
				//enable shell
				select_s = 1;
				break;
			case 'b':
				//enable batcher
				select_b = 1;
				break;
			case 'h':
				//enable heap
				select_h = 1;
				break;
			case 'q':
				//enable quick
				select_q = 1;
				break;
			case 'n':
				//specify array length, default 100
				select_n = 1;
				n_arg = atoi(optarg);
				break;
			case 'p':
				//specify how many elements to print, default 100
				select_p = 1;
				p_arg = atoi(optarg);
				break;
			case 'r':
				//specify seed, default 13371453
				select_r = 1;
				r_arg = atoi(optarg);
				break;
			case '?':
				//unknown parameter
				printf("SYNOPSIS\n   A collection of comparison-based sorting algorithms.\n\nUSAGE\n   ./sorting [-Hasbhqn:p:r:] [-n length] [-p elements] [-r seed]\n\nOPTIONS\n   -H              Display program help and usage.\n   -a              Enable all sorts.\n   -s              Enable Shell Sort.\n   -b              Enable Batcher Sort.\n   -h              Enable Heap Sort.\n   -q              Enable Quick Sort.\n   -n length       Specify number of array elements (default: 100).\n   -p elements     Specify number of elements to print (default: 100).\n   -r seed         Specify random seed (default: 13371453).\n");
				return 0;
		}
	}
	if (select_n == 1 || select_p == 1 || select_r == 1) {
		//n or p or r parameter detected, print name of each sort followed by stats, with array elements in the next line
		//subject to restrictions according to each parameter's argument
		//if only these parameters provided, print error and default msg
		if (select_a == 0) {
			if (select_s == 0) {
				if (select_b == 0) {
					if (select_h == 0) {
						if (select_q == 0) {
							printf("SYNOPSIS\n   A collection of comparison-based sorting algorithms.\n\nUSAGE\n   ./sorting [-Hasbhqn:p:r:] [-n length] [-p elements] [-r seed]\n\nOPTIONS\n   -H              Display program help and usage.\n   -a              Enable all sorts.\n   -s              Enable Shell Sort.\n   -b              Enable Batcher Sort.\n   -h              Enable Heap Sort.\n   -q              Enable Quick Sort.\n   -n length       Specify number of array elements (default: 100).\n   -p elements     Specify number of elements to print (default: 100).\n   -r seed         Specify random seed (default: 13371453).\n");
							return 0;
						}
					}
				}
			}
		}
		/*
		//dynamic memory allocation
		uint32_t *Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
		//set seed
		srandom(r_arg);
		//fill the array with random data
  		for (int i = 0; i < n_arg; i += 1) {
    			Array[i] = random() % 1000;
  		}
  		//printing
  		for (int i = 0; i < p_arg; i += 1) {
  			if (i == n_arg) {
  				break;
  			}
    			printf("%13" PRIu32 "", Array[i]);
		}
		free(Array);
		*/
	} //else {
		//no n or p or r parameter activated, check if -a is activated
		free(Array);
		if (select_a == 1) {
			//for shell sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			shell_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Shell Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
			
			//for batcher sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			batcher_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Batcher Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
			
			//for heap sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			heap_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Heap Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
			
			//for quick sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			quick_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Quick Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
			return 0;
		}
		//if shell selected
		if (select_s == 1){
			//for shell sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			shell_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Shell Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
		}
		//if batcher selected
		if (select_b == 1){
			//for batcher sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			batcher_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Batcher Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
		}
		//if heap selected
		if (select_h == 1){
			//for heap sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			heap_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Heap Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
		}
		//if quick selected
		if (select_q == 1){
			//for quick sort
			//dynamic memory allocation
			Array = (uint32_t *)calloc(n_arg, sizeof(uint32_t));
			srandom(r_arg); /* Set the seed */
			//fill the array with random data
  			for (int i = 0; i < n_arg; i += 1) {
    				Array[i] = random();
  			}
  			//sorting
			reset(&Lstats);
  			quick_sort(&Lstats, Array, n_arg);
  			//printing
  			nums = 0;
  			printf("Quick Sort, %i elements, %lu moves, %lu compares\n", n_arg, Lstats.moves, Lstats.compares);
  			for (int i = 0; i < p_arg; i += 1) {
  				if (i == n_arg) {
  					break;
  				}
  				if (nums % 5 == 0) {
  					if (nums != 0) {
  						printf("\n");
  					}
  				}
  				nums += 1;
    				printf("%13" PRIu32 "", Array[i]);
			}
			printf("\n");
			free(Array);
		}
		return 0;
	//}
	return 0;
}
