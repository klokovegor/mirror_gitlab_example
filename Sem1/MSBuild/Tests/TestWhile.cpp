#include "gtest/gtest.h"
#include "WhileTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>



TEST(TestFunSqCom, While71) {
	int n = 4;

	int f = sqare_comparison(n);

	ASSERT_EQ(f, 2);
}

TEST(TestOnPositiveSq, While72) {
	int n = 0;

	int f = sqare_comparison(n);

	ASSERT_EQ(f, 0);
}



TEST(TestFunPowOfTwo, While51) {
	int n = 8;

	int f = sqare_comparison(n);

	ASSERT_EQ(f, 3);
}

TEST(TestOnPositivePow, While52) {
	int n = 0;

	int f = sqare_comparison(n);

	ASSERT_EQ(f, 0);
}



TEST(TestFunQuanSqInRec, While301) {
	int A = 7;
	int B = 4;
	int C = 2;

	int f = quantity_square_in_rectangle(A, B, C);

	ASSERT_EQ(f, 2);
}

TEST(TestPosiveSides, While302) {
	int A = 0;
	int B = 6;
	int C = 2;

	int f = quantity_square_in_rectangle(A, B, C);

	ASSERT_EQ(f, 0);
}
