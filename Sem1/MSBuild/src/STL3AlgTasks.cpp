#include "STL3AlgTasks.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>
#include <deque>


std::vector<int> del_second_null(std::vector<int>& v) {

	auto it = std::find(v.begin(), v.end(), 0);

	if (it != v.end()) {
		++it;
		auto iter = std::find(it, v.end(), 0);

		if (iter != v.end()) {
			iter = v.erase(iter);
		}
	}

	return v;
}

std::list<int> double_last_elem(const std::vector<int>& v, std::list<int>& l) {
	std::list<int> error = {};

	if (v.size() % 2 != 0) {
		return error;
	}

	auto it = v.begin();
	std::advance(it, v.size() / 2 - 1);
	auto l1 = l.end();
	--l1;

	auto res = std::find_first_of(l1, l.end(), v.begin(), it);

	if (res != l.end()) {
		l.insert(l.end(), *res);
	}

	return l;
}

std::vector<int> resets_the_first_pair(std::vector<int>& v) {
	auto i = std::adjacent_find(v.begin(), v.end(),
		[](int n1, int n2) { return (n1 % 2 == n2 % 2); });

	if (i != v.end()) {
		*i = 0;
		++i;
		*i = 0;
	}

	return v;
}

std::deque<int> front_and_back_insert(int N, std::deque<int>& d) {

	if (N <= 0) {
		return d;
	}

	{
		int n = N;
		std::generate_n(std::front_inserter(d), N,
			[&n]() { return n--; });
	}

	{
		int n = N;
		std::generate_n(std::back_inserter(d), N,
			[&n]() { return n--; });
	}

	return d;
}

std::list<int> left_shift(int K, std::list<int>& l1) {
	if ((K <= 0) || (K >= 10)) {
		return l1;
	}

	if (l1.size() < 10) {
		return l1;
	}

	auto it1 = l1.begin();
	std::advance(it1, K);

	std::rotate(l1.begin(), it1, l1.end());

	return l1;
}

std::list<int> right_shift(int K, std::list<int>& l2) {
	if ((K <= 0) || (K >= 10)) {
		return l2;
	}

	if (l2.size() < 10) {
		return l2;
	}

	auto it2 = l2.rbegin();
	std::advance(it2, K);

	std::rotate(l2.begin(), it2.base(), l2.end());

	return l2;
}

