#include "hinhchunhat.h"
#include <math.h>

const double EPSILON = 1e-6;
double deltat = 0.0001;

double f(double x, double width, double length) {
	return x * (width - 2 * x) * (length - 2 * x);
}

double df(double x, double width, double length) {
	return (f(x + deltat, width, length) - f(x - deltat, width, length)) / (2 * deltat);
}

double timx(double width, double length) {
	double x = 0, y = f(x, width, length), dy, y0;
	double ALPHA = 0.00001;
	int NMAX = 100000;
	int n = 0;

	while (n < NMAX) {
		y0 = y;
		dy = df(x, width, length);

		if (dy >= 0)
			x = x + ALPHA * dy;
		else
			x = x - ALPHA * dy;

		if (x >= width / 2 || x >= length / 2)
		{
			return x;
			break;
		}

		y = f(x, width, length);
		n++;
		if (fabs(y - y0) < EPSILON)
			break;
	}
	return x;
}
