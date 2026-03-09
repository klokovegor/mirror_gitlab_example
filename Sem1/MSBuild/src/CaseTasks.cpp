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
		return 3;
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
	std::wcout.imbue(std::locale("rus_rus.866"));

	switch (number_of_day) {
	case 1:
		return L"понедельник";
	case 2:
		return L"вторник";
	case 3:
		return L"среда";
	case 4:
		return L"четверг";
	case 5:
		return L"пятница";
	case 6:
		return L"суббота";
	case 7:
		return L"воскресенье";
	default:
		return L"Ошибочный день недели ";
	}
}

