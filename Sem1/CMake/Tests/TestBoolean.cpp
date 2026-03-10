#include "gtest/gtest.h"
#include "BooleanTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>

TEST(TestFunPositivenum, Boolean141) {
	int a = -8;
	int b = 23;
	int c = 0;
	
	bool t = positive_num(a, b, c);

	ASSERT_TRUE(t);
}

TEST(TestSameValyes, Boolean142) {
	int a = 5;
	int b = 5;
	int c = 5;

	// ˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜˜˜ ˜˜˜˜˜
	bool t = (a == b && b == c);

	ASSERT_TRUE(t);
}

TEST(TestFunOperation, Boolean51) {
	int a = 2;
	int b = 44;
	
	bool t = operation(a, b);

	ASSERT_TRUE(t);
}

TEST(TestBetweenComparios, Boolean52) {
	long long a = 19999999990LL;
	long long b = 44;

	bool t = operation(a, b);

	ASSERT_TRUE(t);
}


TEST(TestFunFlip, Boolean231) {
	int a = 1111;
	
	bool t = flip(a);
	
	ASSERT_TRUE(t);
}

TEST(TestNumFour, Boolean232) {
	int t = 1111;

	//˜˜˜˜˜˜˜˜˜, ˜˜˜ ˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜˜˜˜˜
	ASSERT_GE(t, 1000);
	ASSERT_LE(t, 9999);
}

//˜˜˜˜˜˜˜˜˜
