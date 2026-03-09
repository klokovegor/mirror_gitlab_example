#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <numeric>
#include <vector>
#include <iterator>
#include "STL1Iter15.h"



// STL1Iter15°. Дана строка name и набор целых чисел. 
// Записать в текстовый файл с именем name все числа из исходного набора в том же порядке, 
// заменяя каждое число 0 на число 10 и добавляя после каждого числа два пробела. 
// Использовать итераторы ptin_iterator, ostream_iterator и алгоритм replace_copy.


/// @brief Перезаписывает файл name на значения из контейнера
/// @param name name2 Файл name для записи
/// @param res Контейнер данных
void overwriting(std::ostream& name, std::vector<int> res) {

    std::ostream_iterator<int> output_file(name, "  ");

    std::copy(res.begin(), res.end(),
        output_file);
}


/// @brief Перезаписывает в файл строку удваивая пробелы и заменяя 0 на 10
/// @return Контейнер чисел из изменённой строки
std::vector<int> list_with_change(std::istream& num, std::ostream& name) {

    std::vector<int> result;

    std::istream_iterator<int> iter(num);
    std::istream_iterator<int> iter_end;

    std::replace_copy(iter, iter_end,
        back_inserter(result),
        0,
        10);

    overwriting(name, result);

    return result;
}
