#pragma once

#include <iostream>
#include <tuple>
#include <cmath>

/// @brief выводит количество дней в невисокосном году
/// @param number_of_month номер месяца 1 - январь, ..., 12 - декабрь
int number_of_month1(int number_of_month);

/// @brief калькулятор с базовыми командами "+" "-" "*" "/"
/// @param N номер команды
/// @param A вещественное число
/// @param B вещественное число
/// @return вещественное число
double calc(int N, double A, double B);

/// @brief Печатает день недели
/// @param number_of_month 1 - понедельник, ..., 7 - воскресенье
std::wstring number_of_day1(int number_of_day);

