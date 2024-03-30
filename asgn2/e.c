#include <stdio.h>
#include "mathlib.h"

int e_count;

double e() {
	int itrs = 100;
	double e_value = 1.0, numerator = 1.0, denominator = 1.0;
	for (e_count = 1; e_count < itrs; e_count++) {
        	denominator *= e_count;
        	numerator = 1 / denominator;
        	e_value += numerator;
	}
	return e_value;
}

int e_terms() {
	return e_count;
}

