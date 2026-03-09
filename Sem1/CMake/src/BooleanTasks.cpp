#include "BooleanTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>

bool positive_num(int a, int b, int c) {
	bool s = (a > 0 && b <= 0 && c <= 0) || (b > 0 && a <= 0 && c <= 0) || (c > 0 && b <= 0 && a <= 0);

	return s;
}

bool operation(long long A, long long B) {
	bool s = A >= 0 || B < -2;

	return s;
}

bool flip(int a) {
	int b = ((a - a % 1000) / 1000) + ((a % 1000 - a % 100) / 10) + ((a % 100 - a % 10) * 10) + (a % 10 * 1000);

	bool s = (a == b);

	return s;
}