#include <stdio.h>
#include "mathlib.h"

int viete_count;

double pi_viete() {
	int itrs = 100;
	double term = sqroot(2), value = sqroot(2) / 2;
	for (viete_count = 0; viete_count < itrs; viete_count++) {
        	term = sqroot(2 + term);
        	value *= term / 2;
	}
	return 1 / (value / 2);
}

int pi_viete_factors() {
	return viete_count;
}

