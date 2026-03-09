#include "gtest/gtest.h"
#include "STL1IterTasks.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>



// STL1Iter10°. Дан набор вещественных чисел, содержащий не менее двух элементов. 
// Вывести числа из исходного набора с четными порядковыми номерами (т. е. второе число, четвертое число и т. д.). 
// Использовать итераторы ptin_iterator, ptout_iterator и алгоритм remove_copy_if.

/// @brief Проверка на корректность вывода, смотря на размерность вектора
TEST(iter10, BaseSizeTest) {

    std::vector<double> root{ 1.111, 2.222, 3.333, 4.444, 5.555 };
    std::vector<double> result{ 2.222, 4.444 };

    std::vector<double> v = even_list_with_copy_if(root);

    bool t = v.size() == result.size();
    ASSERT_TRUE(t);
}

/// @brief Проверка условия if
TEST(Iter10, TestIf) {

    std::vector<double> root{ 4.444 };
    std::vector<double> result{ 4.444 };

    std::vector<double> v = even_list_with_copy_if(root);

    bool t = v.size() == root.size();
    ASSERT_TRUE(t);
}



// STL1Iter14°. Решить задачу STL1Iter10, используя вместо алгоритма remove_copy_if цикл for с параметром-итератором.
// Указание.Ср.с задачей STL1Iter13.В данном случае надо использовать цикл с параметром in типа ptin_iterator.




/// @brief Проверка на корректность вывода, смотря на размерность вектора
TEST(Iter14, BaseSizeTest) {

    std::vector<double> root{ 1.111, 2.222, 3.333, 4.444, 5.555 };
    std::vector<double> result{ 2.222, 4.444 };

    std::vector<double> v = even_list_with_for(root);

    bool t = v.size() == result.size();
    ASSERT_TRUE(t);
}

/// @brief Проверка условия if
TEST(Iter14, TestIf) {

    std::vector<double> root{ 2.222 };
    std::vector<double> result{ 2.222 };

    std::vector<double> v = even_list_with_for(root);

    bool t = v.size() == root.size();
    ASSERT_TRUE(t);
}



// STL1Iter5°. Дан текстовый файл с именем name, содержащий строковые представления целых чисел.  
// Вывести все числа из файла в том же порядке.
// Использовать итераторы istream_iterator, ptout_iterator и алгоритм copy.



/// @brief Проверка на выводимый контейнер
TEST(Iter5, VectorCorrect) {

    std::vector<int> t = { 1, 10, 3, 55, 0, 23, 33 };
    std::ifstream file("name.txt");

    std::vector<int> v = list_from_file(file);
    file.close();

    bool s = v == t;
    ASSERT_TRUE(s);
}

/// @brief Прповерка на нулевое значение контейнера
TEST(Iter5, NullValue) {
   
    std::ifstream file("name.txt");

    std::vector<int> v = list_from_file(file);
    file.close();

    bool s = v.size() == 0;
    ASSERT_TRUE(s);
}



// STL1Iter15°. Дана строка name и набор целых чисел. 
// Записать в текстовый файл с именем name все числа из исходного набора в том же порядке, 
// заменяя каждое число 0 на число 10 и добавляя после каждого числа два пробела. 
// Использовать итераторы ptin_iterator, ostream_iterator и алгоритм replace_copy.




/// @brief Проверка на корректность создаваемых значений в векторе
TEST(Iter15, CorrectVectorTest) {
    std::vector<int> t = { 1, 10, 3, 55, 10, 23, 33 };

    std::ofstream file("name.txt");

    std::string num = "1 10 3 55 0 23 33";
    std::stringstream sstream(num);

    std::vector<int> res = list_with_change(sstream, file);

    file.flush();
    file.close();

    bool s = res == t;
    ASSERT_TRUE(s);
}

/// @brief Проверка и на значения и на пробелы
TEST(Iter15, StringCorrect) {
    std::string t = "1  10  3  55  10  23  33";

    std::ofstream file("name.txt");

    std::string num = "1 10 3 55 0 23 33";
    std::stringstream sstream(num);

    std::vector<int> res = list_with_change(sstream, file);

    file.flush();
    file.close();

    std::ifstream file_u("name.txt");
    std::istream_iterator<std::string> it1(file_u);
    std::istream_iterator<std::string> it1_end;

    std::string receive_str;

    std::getline(file_u, receive_str);

    file.close();

    ASSERT_EQ(t, receive_str);
}



// STL1Iter24°. Дан текстовый файл с именем name1, содержащий английские слова,  
// строка name2 и набор английских слов, размер которого не превосходит количество слов, содержащихся в файле name1.
// Записать в текстовый файл с именем name2 все слова из исходного набора, 
// дополнив каждое слово символом «-» (дефис) и словом из файла name1 с тем же порядковым номером 
// (если файл name1 содержит больше слов, чем исходный набор, то лишние слова в файле игнорируются). 
//Каждое дополненное слово записывать в файл с новой строки. 
// Использовать итераторы ptin_iterator, istream_iterator, ostream_iterator и алгоритм transform.
// Указание.Рекомендации по использованию функциональных объектов приведены в указании к STL1Iter2.





/// @brief Проверка на корректность создаваемых значений в векторе
TEST(Iter24, CorrectVectorTest) {
    std::vector<std::string> s = { "hello-hi", "bye-seeyou", "forest-wood", "run-guy" };
    std::string word = "hi seeyou wood guy";
    std::stringstream sstream(word);
    std::ifstream file1("name1.txt");
    std::ofstream file2("name2.txt");

    std::vector<std::string> v = change_file(file1, sstream, file2);

    file1.close();
    file2.flush();
    file2.close();

    bool x = s == v;
    ASSERT_TRUE(x);
}

/// @brief Проверка и на значения и на пробелы
TEST(Iter24, StringCorrect) {
    // Arrange
    std::string x = "hello-hi bye-seeyou forest-wood run-guy";
    std::string word = "hi seeyou wood guy";
    std::stringstream sstream(word);
    std::ifstream file1("name1.txt");
    std::ofstream file2("name2.txt");

    // Act
    change_file(file1, sstream, file2);

    // Assert
    std::ifstream file_u("name1.txt");
    std::istream_iterator<std::string> it1(file_u);
    std::istream_iterator<std::string> it1_end;

    std::string receive_str;

    std::getline(file_u, receive_str);

    file1.close();
    file2.flush();
    file2.close();
    file_u.close();

    ASSERT_EQ(x, receive_str);
}

