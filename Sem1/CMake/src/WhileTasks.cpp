#include "WhileTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>

int sqare_comparison(int N) {
	int K = 1;

	if (N <= 0) {
		return 0;
	}

	while (K * K <= N) {
		++K;
	}

	return K - 1;
}

int pow_of_two(int N) {
	int K = 0;

	if (N <= 0) {
		return 0;
	}

	while (pow(2, K) != N) {
		++K;
	}

	return K;
}

int quantity_square_in_rectangle(int A, int B, int C) {
	int t = 0;
	int a = A;
	int b = B;

	if ((A <= 0) && (B <= 0)) {
		return t;
	}

	while (((a - C) >= 0) && ((b - C) >= 0)) {
		++t;
		a = a - C;
		b = b - C;
	}

	return t;
}

