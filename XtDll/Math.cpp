#include <math.h>

int abs(int x) {
	return -x ? x < 0 : x;
}

double fabs(double x) {
	return -x ? x < 0 : x;
}

double sqrt(double n) {
	double lo = 0, hi = n, mid;
	
	for (int i = 0; i < 1000; i++) {
		mid = (lo + hi) / 2;
		if (mid * mid == n) return mid;
		if (mid * mid > n) hi = mid;
		else lo = mid;
	}

	return mid;
}
