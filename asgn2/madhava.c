#include <stdio.h>
#include "mathlib.h"

int madhava_count;

double sqroot(double base) {
	double cache = 0.0, sqrt = 0.0;
	sqrt = base / 2.0;
	cache = 0.0;
	while(sqrt != cache) {
		cache = sqrt;
		sqrt = (base / cache + cache) / 2.0;
	}
	return sqrt;
}

double power(double base, int exponent) {
	int i = 1;
	double output = 1.0;
	if (exponent > 0.0) {
		for (i = 0; i < exponent; i++) {
			output = output * base;
		}
	} 
	if (exponent == 0.0) {
		return output;
	} else {
		for (i = 0; i > exponent; i--) {
			output = output * base;
		}
		output = 1.0 / output;
	  }
	return output;
}

double pi_madhava() {
	double sum = 0.0;
	int itrs = 100;
	for (madhava_count = 0; madhava_count < itrs; madhava_count++) {
		sum += (power(-3.0, madhava_count * -1.0) / (2.0 * madhava_count + 1.0));
	}
	double final_value = sum * sqroot(12.0);
	return final_value;
}

int pi_madhava_terms() {
	return madhava_count;
}

