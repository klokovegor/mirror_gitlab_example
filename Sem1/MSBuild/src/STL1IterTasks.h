#pragma once

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>


/// @brief Создаёт контейнер, в котором находятся элементы, стоящие на чётных местах изначального контейнере при помощи copy_if
/// @param name Контейнер, который нужно преобразовать
/// @return Контейнер с числами, изначально
std::vector<double> even_list_with_copy_if(const std::vector<double>& name);

/// @brief Создаёт контейнер, в котором находятся элементы, стоящие на чётных местах изначального контейнере при помощи цикла for
/// @param name Контейнер, который нужно преобразовать
/// @return Контейнер с числами, изначально
std::vector<double> even_list_with_for(const std::vector<double>& name);

/// @brief Считывает с файла строку, которую преобразовывает в контейнер с числами
/// @param name Исходный файл
/// @return Контейнер с целыми числами
std::vector<int> list_from_file(std::istream& name);

/// @brief Перезаписывает файл name на значения из контейнера
/// @param name name2 Файл name для записи
/// @param res Контейнер данных
void overwriting(std::ostream& name, std::vector<int> res);

/// @brief Перезаписывает в файл строку удваивая пробелы и заменяя 0 на 10
/// @return Контейнер чисел из изменённой строки
std::vector<int> list_with_change(std::istream& num, std::ostream& name);

/// @brief Перезаписывает файл name2 на значения из контейнера
/// @param name2 Файл name2 для записи
/// @param res Контейнер данных
void record(std::ostream& name2, std::vector<std::string> res);

/// @brief Создаёт во втором файле группу из слов
/// @param name1 Файл name1 для чтения
/// @param name2 Файл name2 для чтения
/// @param name3 Файл name2 для записи
/// @return Контейнер данной группы слов
std::vector<std::string> change_file(std::istream& name1, std::istream& words, std::ostream& name2);
