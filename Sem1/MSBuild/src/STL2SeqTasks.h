#pragma once

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>
#include <deque>

/// @brief Считывает с файла строку, которую преобразовывает в вектор с числами
/// @param name Исходный файл
/// @return Вектор с целыми числами
std::vector<int> file_from_vector(std::istream& name);

/// @brief Добавляет из дека в список на определённое место
/// @param d Дек
/// @param l Список
/// @return Изменённый список
std::list<int> add_dec_in_list(std::deque<int>& d, std::list<int> l);

/// @brief Добавляет после каждого значения вектора -1
/// @param v Изначальный вектор
/// @return Изменённый вектор
std::vector<int> add_one_in_vector(std::vector<int>& v);

/// @brief Удаляет элементы исходного вектора с нечётным порядковым номерами
/// @param v Исходный вектор
/// @return Изменённый вектор
std::vector<int> delele_uneven_value(std::vector<int>& v);

/// @brief Удаляет в первой половине исходного списка все элементы с нечетными порядковыми номерами
/// @param l Ссходный список
/// @return Изменённый список
std::list<int> del_part_of_list(std::list<int> l);

