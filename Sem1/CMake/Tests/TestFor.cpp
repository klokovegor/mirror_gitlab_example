#include "gtest/gtest.h"
#include "ForTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>



TEST(TestFunNumerical_sequence, For251) {
	double a = 0.25;
	int b = 3;

	double t = numerical_sequence(a, b);

	double r = std::abs(t - std::log(4));
	bool are_different2 = r > std::numeric_limits<double>::epsilon();
	ASSERT_TRUE(are_different2);
}

TEST(TestPositiveSum, For252) {
	double a = 0.25;
	int b = 3;

	double t = numerical_sequence(a, b);

	// Проверяем сумму на свой знак
	ASSERT_GE(t, 0);
}



TEST(TestFunPrintSeq, For341) {
	int a = 3;

	std::vector<int> etalon{1, 2, 1};

	std::vector<int> res = print_seqence(a);

	ASSERT_EQ(etalon, res);
}

TEST(TestOnIncorrect, For342) {
	int a = 1;

	std::vector<int> etalon{};

	std::vector<int> res = print_seqence(a);

	ASSERT_EQ(etalon, res);
}



TEST(TestFunPrSeq, For401) {
	int a = 1;
	int b = 3;

	std::vector<int> etalon{1, 2, 2, 3, 3, 3};

	std::vector<int> res = print_seqence(a, b);

	ASSERT_EQ(etalon,res);
}

TEST(TestNotTrueCondition, For402) {
	int a = 3;
	int b = 1;

	std::vector<int> etalon{};

	std::vector<int> res = print_seqence(a, b);

	ASSERT_EQ(etalon, res);
}