#include "CaseTasks.h"

#include <iostream>
#include <tuple>
#include <cmath>

int number_of_month1(int number_of_month) {
	switch (number_of_month) {
	case 1:
		return 31;
	case 2:
		return 28;
	case 3:
		return 31;
	case 4:
		return 30;
	case 5:
		return 31;
	case 6:
		return 30;
	case 7:
		return 31;
	case 8:
		return 31;
	case 9:
		return 30;
	case 10:
		return 31;
	case 11:
		return 30;
	case 12:
		return 31;
	default:
		return 0;
	}
}

double calc(int N, double A, double B) {
	double result;
	switch (N) {
	case 1:
		result = A + B;
		return result;
	case 2:
		result = A - B;
		return result;
	case 3:
		result = A * B;
		return result;
	case 4:
		result = A / B;
		return result;
	default:
		result = std::numeric_limits<double>::quiet_NaN();
		return result;
	}
}

std::wstring number_of_day1(int number_of_day) {

	switch (number_of_day) {
	case 1:
		return L"\u043f\u043e\u043d\u0435\u0434\u0435\u043b\u044c\u043d\u0438\u043a";
	case 2:
		return L"\u0432\u0442\u043e\u0440\u043d\u0438\u043a";
	case 3:
		return L"\u0441\u0440\u0435\u0434\u0430";
	case 4:
		return L"\u0447\u0435\u0442\u0432\u0435\u0440\u0433";
	case 5:
		return L"\u043f\u044f\u0442\u043d\u0438\u0446\u0430";
	case 6:
		return L"\u0441\u0443\u0431\u0431\u043e\u0442\u0430";
	case 7:
		return L"\u0432\u043e\u0441\u043a\u0440\u0435\u0441\u0435\u043d\u044c\u0435";
	default:
		return L"\u041e\u0448\u0438\u0431\u043e\u0447\u043d\u044b\u0439 \u0434\u0435\u043d\u044c \u043d\u0435\u0434\u0435\u043b\u0438 ";
	}
}

