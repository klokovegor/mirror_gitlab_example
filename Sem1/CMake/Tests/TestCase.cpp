#include "gtest/gtest.h"
#include "CaseTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>

/// @brief выводит количество дней в невисокосном году
/// @param number_of_month номер месяца 1 - январь, ..., 12 - декабрь
int number_of_month1(int number_of_month);

TEST(TestNumOfMonth, Case41) {
	int m = 2;
	
	int f = number_of_month1(m);

	bool r = f == 28;
	ASSERT_TRUE(r);
}

TEST(TestOnIntNum, Case42) {
	int m = 13;

	int f = number_of_month1(m);

	bool r = f == 0;
	ASSERT_TRUE(r);
}



TEST(TestFunCalc, Case51) {
	int N = 3;
	double A = 3;
	double B = 3;

	double r = calc(N, A, B);

	bool f = r == 9;
	ASSERT_TRUE(f);
}

TEST(TestOnOper, Case52) {
	int N = 5;
	double A = 3;
	double B = 3;

	double r = calc(N, A, B);

	ASSERT_TRUE(std::isnan(r));
}



TEST(TestFunNumOfDay, Case11) {
	int N = 4;
	
	std::wstring s = number_of_day1(N);

	bool is_correct = s == L"четверг";
	ASSERT_TRUE(is_correct);
}

TEST(TestOnNumDay, Case12) {
	int N = 8;

	std::wstring s = number_of_day1(N);

	bool is_incorrect = s == L"Ошибочный день недели ";
	ASSERT_TRUE(is_incorrect);
}

// Проверка на изменение