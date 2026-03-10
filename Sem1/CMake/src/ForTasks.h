#pragma once

#include <iostream>
#include <tuple>
#include <cmath>
#include <vector>


/// @brief Сумма числовой последовательности F25
/// @param x вещественное число |x| < 1
/// @param N целочисленное число
/// @return сумму
double numerical_sequence(double x, int N);

/// @brief выводит последовательность, заданной по данной формуле
/// @param N порядковое число
/// @return массив
std::vector<int> print_seqence(int N);

/// @brief Выводит числовую последовательность с последовательным увеличение числа повторений на 1
/// @param A целочисленное число
/// @param B целочисленное число
/// @return массив
std::vector<int> print_seqence(int A, int B);

