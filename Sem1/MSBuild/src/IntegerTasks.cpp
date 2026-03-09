#include "IntegerTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>

int integer30(int years) {
	int centery = (years - 1) / 100 + 1;

	return centery;
}

int integer28(int N, int K) {
	int a = (N + K % 7) % 7;

	return a;
}

int integer22(int N) {
	int s = N % 3600;

	return s;
}

