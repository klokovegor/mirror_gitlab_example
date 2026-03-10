#pragma once
#include "STL3AlgTasks.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>
#include <deque>

/// @brief Находит второй ноль в векторе и удаляет его
/// @param v Исходный вектор
/// @return Изменённый вектор
std::vector<int> del_second_null(std::vector<int>& v);

/// @brief Дублирует последний элемент списка, если он присутствует в первой половине вектор
/// @param v Вектор
/// @param l Начальный список
/// @return Изменённый список
std::list<int> double_last_elem(const std::vector<int>& v, std::list<int>& l);

/// @brief Обнуляет первую пару соседних элементов вектора, если они имеют одинаковую чётность
/// @param v Изначальный вектор
/// @return Изменённый вектор
std::vector<int> resets_the_first_pair(std::vector<int>& v);

/// @brief Добавляет в начало и в конец 1, 2, …, N и N, N−1, …, 2, 1 соответственно
/// @param N Целое число
/// @param d Начальный дек
/// @return Изменённый дек
std::deque<int> front_and_back_insert(int N, std::deque<int>& d);

/// @brief Выполняет сдвиг влево на К
/// @param K Целочисленное число 0 < K < 10
/// @param l1 Изначальный список
/// @return Изменённый список
std::list<int> left_shift(int K, std::list<int>& l1);

/// @brief Выполняет сдвиг влево на К
/// @param K Целочисленное число 0 < K < 10
/// @param l2 Изначальный список
/// @return Изменённый список
std::list<int> right_shift(int K, std::list<int>& l2);

