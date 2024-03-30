#include <stdio.h>
#include "mathlib.h"

int newton_count;

double remainder(double x, double y) {
	double value = 0.0;
	value = x - (x / y) * y;
	return value;
}

double sqrt_newton(double argument) {
	newton_count = 0;
	double z = 0.0, y = 1.0;
	while (absolute(y - z) > EPSILON) {
		z = y;
		y = 0.5 * (z + argument / z);
		newton_count += 1;
	}
	return y;
}

int sqrt_newton_iters() {
	return newton_count;
}

