#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#define OPTIONS "aebmrvnsh"
#include "mathlib.h"
#include "madhava.c"
#include "e.c"
#include "bbp.c"
#include "euler.c"
#include "newton.c"
#include "viete.c"

int e_test() {
	printf("e() = %.15f, M_E = %.15f, diff = %.15f\n", e(), M_E, fabs(e() - M_E));
	return 0;
}

int euler_test() {
	printf("pi_euler() = %.15f, M_PI = %.15f, diff = %.15f\n", pi_euler(), M_PI, fabs(pi_euler() - M_PI));
	return 0;
}

int bbp_test() {
	printf("pi_bbp() = %.15f, M_PI = %.15f, diff = %.15f\n", pi_bbp(), M_PI, fabs(pi_bbp() - M_PI));
	return 0;
}

int madhava_test() {
	printf("pi_madhava() = %.15f, M_PI = %.15f, diff = %.15f\n", pi_madhava(), M_PI, fabs(pi_madhava() - M_PI));
	return 0;
}

int viete_test() {
	printf("pi_viete() = %.15f, M_PI = %.15f, diff = %.15f\n", pi_viete(), M_PI, fabs(pi_viete() - M_PI));
	return 0;
}

int newton_test() {
	double itrs = 0.0;
	while (itrs <= 10.0) {
		printf("sqrt_newton(%f) = %f, sqrt(%f) = %f, diff = %f\n", itrs, sqrt_newton(itrs), itrs, sqrt(itrs), fabs(sqrt_newton(itrs) - sqrt(itrs)));
		itrs += 0.1;
	}
	return 0;
}

int newton_test_with_terms() {
	double itrs = 0.0;
	while (itrs <= 10.0) {
		printf("sqrt_newton(%f) = %f, sqrt(%f) = %f, diff = %f\n", itrs, sqrt_newton(itrs), itrs, sqrt(itrs), fabs(sqrt_newton(itrs) - sqrt(itrs)));
		printf("sqrt_newton() terms = %d\n", newton_count);
		itrs += 0.1;
	}
	return 0;
}

int main(int argc, char **argv) {
	int opt = 0;
	if (argc == 1) {
		printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
		return 0;
	}
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		if (opt == 97) {
			if (argv[optind] == NULL){
				e_test();
				euler_test();
				bbp_test();
				madhava_test();
				viete_test();
				newton_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					e_test();
					printf("e() terms = %d\n", e_count);
					euler_test();
					printf("pi_euler() terms = %d\n", pi_euler_terms());
					bbp_test();
					printf("pi_bbp() terms = %d\n", bbp_count);
					madhava_test();
					printf("pi_madhava() terms = %d\n", madhava_count);
					viete_test();
					printf("pi_viete() terms = %d\n", viete_count);
					newton_test_with_terms();
					break;
				}
				
			}
		}
		if (opt == 101) {
			if (argv[optind] == NULL){
				e_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					e_test();
					printf("e() terms = %d\n", e_count);
					break;
				}
				e_test();
			}
		} 
		if (opt == 98) {
			if (argv[optind] == NULL){
				bbp_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					bbp_test();
					printf("pi_bbp() terms = %d\n", bbp_count);
					break;
				}
				bbp_test();
			}
		} 
		if (opt == 109) {
			if (argv[optind] == NULL){
				madhava_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					madhava_test();
					printf("pi_madhava() terms = %d\n", madhava_count);
					break;
				}
				madhava_test();
			}
		} 
		if (opt == 114) {
			if (argv[optind] == NULL){
				euler_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					euler_test();
					printf("pi_euler() terms = %d\n", euler_count);
					break;
				}
				euler_test();
			}
		} 
		if (opt == 118) {
			if (argv[optind] == NULL){
				viete_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					viete_test();
					printf("pi_viete() terms = %d\n", viete_count);
					break;
				}
				viete_test();
			}
		} 
		if (opt == 110) {
			if (argv[optind] == NULL){
				newton_test();
			} else {
				if (strcmp(argv[optind], "-h") == 0) {
					printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
					break;
				}
				if (strcmp(argv[optind], "-s") == 0) {
					newton_test_with_terms();
					break;
				}
				newton_test();
			}
		}
		if (opt == 104) {
			printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
			break;
		}
	/**
		switch (opt) {
		case 'a':
			if (optarg == NULL) {
				e_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					printf("e() terms = %d\n", e_count);
				}
			}
			if (optarg == NULL) {
				euler_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					euler_test();
					printf("pi_euler() terms = %d\n", pi_euler_terms());
				}
			}
			if (optarg == NULL) {
				bbp_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					bbp_test();
					printf("pi_bbp() terms = %d\n", bbp_count);
				}
			}
			if (optarg == NULL) {
				madhava_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					madhava_test();
					printf("pi_madhava() terms = %d\n", madhava_count);
				}
			}
			if (optarg == NULL) {
				viete_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					viete_test();
					printf("pi_viete() terms = %d\n", viete_count);
				}
			}
			
			newton_test();
			break;
		case 'e':
			if (optarg == NULL) {
				e_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					e_test();
					printf("e() terms = %d", e_count);
				}
			}
			break;
		case 'b':
			if (optarg == NULL) {
				bbp_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					bbp_test();
					printf("pi_bbp() terms = %d", bbp_count);
				}
			}
			break;
		case 'm':
			if (optarg == NULL) {
				madhava_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					madhava_test();
					printf("pi_madhava() terms = %d", madhava_count);
				}
			}
			break;
		case 'r':
			if (optarg == NULL) {
				euler_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					euler_test();
					printf("pi_euler() terms = %d", euler_count);
				}
			}
			break;
		case 'v':
			if (optarg == NULL) {
				viete_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					viete_test();
					printf("pi_viete() terms = %d", viete_count);
				}
			}
			break;
		case 'n':
			if (optarg == NULL) {
				newton_test();
			} else {
				if (strcmp(optarg, "-s") == 0) {
					newton_test();
					printf("pi_newton() terms = %d", newton_count);
				}
			}
			break;
		case 'h':
			printf("SYNOPSIS\n   A test harness for the small numerical library.\n\nUSAGE\n   ./mathlib-test\n\nOPTIONS\n  -a   Runs all tests.\n  -e   Runs e test.\n  -b   Runs BBP pi test.\n  -m   Runs Madhava pi test.\n  -r   Runs Euler pi test.\n  -v   Runs Viete pi test.\n  -n   Runs Newton square root tests.\n  -s   Print verbose statistics.\n  -h   Display program synopsis and usage.\n");
			break;
		}
	**/
	}
	
	return 0;
}


