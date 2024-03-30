#include <stdio.h>
#include "mathlib.h"

int bbp_count;

double bbp_power(double base, int exponent) {
	int bbp_count = 1;
	double output = 1.0;
	for (bbp_count = 0; bbp_count < exponent; bbp_count++) {
		output = output * base;
	}
	return output;
}

double pi_bbp() {
	int itrs = 100;
	double pi_value = 0.0;
	for (bbp_count = 0; bbp_count < itrs; bbp_count++) {
		pi_value += bbp_power(1.0 / 16.0, bbp_count) * ((4.0 / (8.0 * bbp_count + 1.0)) - (2.0 / (8.0 * bbp_count + 4.0)) - (1.0 / (8.0 * bbp_count + 5.0)) - (1.0 / (8.0 * bbp_count + 6.0)));
	}
	return pi_value;
}

int pi_bbp_terms() {
	return bbp_count;
}
