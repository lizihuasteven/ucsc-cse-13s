#include <stdio.h>
#include "mathlib.h"

int euler_count;

double euler_power(double base, int exponent) {
	int euler_count = 1;
	double output = 1.0;
	for (euler_count = 0; euler_count < exponent; euler_count++) {
		output = output * base;
	}
	return output;
}

double pi_euler() {
	int itrs = 10000000;
	double value = 0.0;
	for (euler_count = 1; euler_count < itrs; euler_count++) {
		value += (1.0 / euler_power(euler_count, 2.0));
	}
	double pi_approx;
	pi_approx = sqroot(6.0 * value);
	return pi_approx;
}

int pi_euler_terms() {
	return euler_count;
}
