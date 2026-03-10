#pragma once

#include <iostream>
#include <tuple>
#include <cmath>

/// @brief сравнивает квадрат числа с другим число
/// @param N число, с котором будут сравнивать квадрат числа
/// @return наименьшее число удовлетворяющее условию K^2 > N
int sqare_comparison(int N);

/// @brief Узнаёт в какую степень нужно возвести 2, чтобы получить N
/// @param N два в степени K
/// @return степень двойки K
int pow_of_two(int N);

/// @brief узнаёт какое количество квадратов помещается в прямоугольник
/// @param A сторона прямоугольника
/// @param B сторона прямоугольника
/// @param C сторона квадрата
/// @return количество квадратов
int quantity_square_in_rectangle(int A, int B, int C);

