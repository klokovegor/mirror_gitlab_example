#pragma once

#include <iostream>
#include <tuple>
#include <cmath>

/// @brief Решает систему с одной переменной
/// @param x вещественное число
/// @return вещественное число
double sist_for_x(double x);

/// @brief Из трёх чисел выводит среднее из них
/// @param a целочисленное число
/// @param b целочисленное число
/// @param c целочисленное число
/// @return среднее число
int average_value(int a, int b, int c);

/// @brief Сортирует от большего к меньшему
/// @param a целочисленое число
/// @param b целочисленое число
/// @return отсортированная последовательность чисел
std::tuple<int, int> decreasing(int a, int b);


