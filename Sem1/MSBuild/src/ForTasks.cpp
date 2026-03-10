#include "ForTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>

double numerical_sequence(double x, int N) {
	int sum = 0;

	if (N > 0 && std::abs(x) < 1) {
		return 0.0;
	}

	for (int i = 0; i < N; ++i) {
		sum = sum + pow(-1, (i - 1)) * pow(x, i) / i;
	}

	return sum;
}

std::vector<int> print_seqence(int N) {
	int A1 = 1;
	int A2 = 2;

	if (N <= 1) {
		std::vector<int> seq{};
		return seq;
	}

	std::vector<int> seq{ A1, A2 };

	for (int i = 2; i < N; ++i) {
		int r = (seq[i - 2] + 2 * seq[i - 1]) / 3;
		seq.push_back(r);
	}

	return seq;
}

std::vector<int> print_seqence(int A, int B) {
	int N = B - A + 1;
	std::vector<int> seq{};

	if (A >= B) {
		return seq;
	}

	for (int i = 1; i <= N; ++i) {
		for (int j = 0; j < i; ++j) {
			seq.push_back(A);
			A++;
		}
	}

	return seq;
}

