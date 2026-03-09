#include "gtest/gtest.h"
#include "IfTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>


TEST(TestFunSist, If241) {
	double a = -3.14;
	
	double t = sist_for_x(a);

	ASSERT_EQ(t, 1);
}

TEST(TestNoEqNull, If242) {
	double a = -3.14;

	double t = sist_for_x(a);

	// Проверяем, что значение не равно нулю
	ASSERT_NE(t, 0.0);
}



TEST(TestFunAveragevalue, If131) {
	int a = 3;
	int b = 34;
	int c = 2;
	
	int t = average_value(a, b, c);

	ASSERT_EQ(t, 3);
}

TEST(TestComparisonNull, If132) {
	int a = 3;
	int b = 34;
	int c = 2;

	int t = average_value(a, b, c);

	// Сравнение среднего числа с нулём
	ASSERT_EQ(t, 0);
}


TEST(TestFunDecreasing, If81) {
	int a = 3;
	int b = 45;

	auto [x1, x2] = decreasing(a, b);
	
	std::cerr << x1 << " " << x2 << std::endl;
}

TEST(TestEq, If82) {
	int a = 3;
	int b = 45;
	
	auto [x1, x2] = decreasing(a, b);

	// Проверяем равенство данных чисел
	ASSERT_FALSE(x1, x2);
}