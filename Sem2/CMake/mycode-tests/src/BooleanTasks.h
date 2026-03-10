#pragma once




#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>


/// @brief Проверка трёх чисел на данное условие при помощи логических операций
/// @param a целочисленное число
/// @param b целочисленное число
/// @param c целочисленное число
/// @return булевое значение
bool positive_num(int a, int b, int c);

/// @brief Проверяем логическое условие из задачи B5
/// @param A целочисленное число
/// @param B целочисленное число
/// @return булевое значение
bool operation(int A, int B);

/// @brief Сравнивает четырёхзначное число с его перевёрнутым значением
/// @param a целочисленное четырёхзначное число
/// @return булевое значение
bool flip(int a);

