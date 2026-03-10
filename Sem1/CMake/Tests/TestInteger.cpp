#include "gtest/gtest.h"
#include "IntegerTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>



TEST(Integer301, Stoletie) {
	int b = integer30(1801);

	ASSERT_EQ(b, 19);
}
TEST(Integer302, Stoletie) {
	int r = 1801;

	ASSERT_GT(r, 0);
}



TEST(Integer281, Dennedely) {
	int f = integer28(4, 32);

	ASSERT_EQ(f, 2);
}

TEST(Integer282, Dennedely) {
	int f = 4;
	int r = 32;

	ASSERT_GT(f, 0);
	ASSERT_GT(r, 0);
}



TEST(Integer221, Chislosecund) {
	int c = integer22(1463324);

	ASSERT_EQ(c, 1724);
}

TEST(Integer222, Chislosecund) {
	int c = integer22(1463324);

	ASSERT_NE(c, 0);
}
