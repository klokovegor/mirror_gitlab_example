#include "gtest/gtest.h"
#include "STL2SeqTasks.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>
#include <deque>
#include <sstream>



//STL2Seq1°. Дан текстовый файл с именем name, содержащий строковые представления целых чисел. 
//Заполнить вектор V числами из исходного файла и вывести элементы вектора в исходном порядке. 
//Для заполнения вектора использовать итератор istream_iterator и конструктор вектора, 
//для вывода элементов вектора использовать алгоритм copy, применяя его к итераторам полученного вектора и итератору ptout_iterator.




/// @brief Проверка на выводимый контейнер
TEST(Seq5, VectorCorrect) {

    {
        std::ofstream out("name.txt");
        out << "1 10 3 55 0 23 33";
        out.close();
    }
    std::vector<int> t = { 1, 10, 3, 55, 0, 23, 33 };
    std::ifstream file("name.txt");

    std::vector<int> v = file_from_vector(file);
    file.close();

    bool s = v == t;
    ASSERT_TRUE(s);
}

/// @brief Проверка на нулевое значение контейнера
TEST(Seq5, NullValue) {

    std::istringstream file("");

    std::vector<int> v = file_from_vector(file);

    bool s = v.size() == 0;
    ASSERT_TRUE(s);
}



//STL2Seq12°. Даны дек D и список L. Каждый исходный контейнер содержит не менее 5 элементов. 
//Вставить перед пятым с конца элементом списка последние 5 элементов дека в обратном порядке. 
//Использовать один вызов функции-члена insert.




/// @brief Проверка работы добавления
TEST(Seq12, CorrectAdd) {
    std::deque<int> d = { 10, 20, 30, 40, 50 };
    std::list<int> l = { 1, 2, 3, 4, 5 };
    std::list<int> con = { 50, 40, 30, 20, 10, 1, 2, 3, 4, 5 };

    std::list<int> ans = add_dec_in_list(d, l);

    ASSERT_EQ(ans, con);
}

/// @brief Проверка на ограничения в условии
TEST(Seq12, ExeptEror) {
    std::deque<int> d = { 10, 20, 30, 40 };
    std::list<int> l = { 1, 2, 3, 4, 5 };
    std::list<int> con = {  };

    std::list<int> ans = add_dec_in_list(d, l);

    ASSERT_EQ(ans, con);
}



//STL2Seq14°. Дан вектор V. Вставить после каждого элемента исходного вектора число −1. 
//Использовать функцию-член insert в цикле с параметром-итератором.




/// @brief Правильность выводимых данных
TEST(Seq14, CorrectVector) {
    std::vector<int> s = { 1, 2, 3 };
    std::vector<int> control = { 1, -1, 2, -1, 3, -1 };

    std::vector<int> answer = add_one_in_vector(s);

    ASSERT_EQ(answer, control);
}

/// @brief Пустой вектор
TEST(Seq14, NullVector) {
    std::vector<int> s = {  };
    std::vector<int> control = {  };

    std::vector<int> answer = add_one_in_vector(s);

    ASSERT_EQ(answer, control);
}




//STL2Seq27°. Дан вектор V. Удалить все элементы исходного вектора с нечетными порядковыми номерами 
//(считая, что начальный элемент вектора имеет порядковый номер 1). 
//Использовать функцию-член erase в цикле с параметром-итератором.




/// @brief  Правильность выводимых данных
TEST(Seq27, TestDel) {
    std::vector<int> s = { 1, 2, 3 };
    std::vector<int> control = { 2 };

    std::vector<int> answer = delele_uneven_value(s);

    ASSERT_EQ(answer, control);
}

/// @brief Пустой вектор
TEST(Seq27, NullVec) {
    std::vector<int> s = {  };
    std::vector<int> control = {  };

    std::vector<int> answer = delele_uneven_value(s);

    ASSERT_EQ(answer, control);
}



//STL2Seq31°. Дан список L с количеством элементов, кратным 4. 
//Удалить в первой половине исходного списка все элементы с нечетными порядковыми номерами 
//(считая, что начальный элемент списка имеет порядковый номер 1). 
//Использовать функцию-член erase в цикле с параметром-итератором.
//STL2Seq32°. Решить задачу STL2Seq31, используя функцию-член erase в цикле по обратному итератору.




/// @brief Работоспособность на примере
TEST(Seq32, Root) {
    std::list<int> s = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::list<int> con = { 2, 4, 5, 6, 7, 8 };

    std::list<int> ans = del_part_of_list(s);

    ASSERT_EQ(ans, con);
}

/// @brief Проверка на кратность
TEST(Seq32, NotFour) {
    std::list<int> s = { 1, 2, 3, 4, 5 };
    std::list<int> con = {  };

    std::list<int> ans = del_part_of_list(s);

    ASSERT_EQ(ans, con);
}
