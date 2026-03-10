
#include "IfTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>

double sist_for_x(double x) {
	if (x > 0) {
		return 2 * sin(x);
	}
	if (x < 0) {
		return 1.0;
	}
	return 6.0 - x;
}

int average_value(int a, int b, int c) {
	if (a > b) {
		if (a > c) {
			if (b > c) {
				return b;
			}
			else {
				return c;
			}
		}
		else {
			return a;
		}
	}
	else {
		if (b > c) {
			if (c > a) {
				return c;
			}
			else {
				return a;
			}
		}
		else {
			return b;
		}
	}
}

std::tuple<int, int> decreasing(int a, int b) {
	if (a > b) {
		return std::make_tuple(a, b);
	}
	else {
		return std::make_tuple(b, a);
	}
}

