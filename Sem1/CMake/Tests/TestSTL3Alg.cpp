#include "gtest/gtest.h"
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



//STL3Alg1°. Дан вектор V. Удалить второй из элементов вектора, равных нулю. 
//Если нулевых элементов меньше двух, то вектор не изменять. 
//Использовать два вызова алгоритма find и функцию-член erase.

// TODO: Сделать константную ссылку



/// @brief Проверка, когда > 1, "0"
TEST(Alg1, Change) {
	std::vector<int> elem = { 23, 43, 0, 11, 6, 26, 0, 6345, 145, 784 };
	std::vector<int> control = { 23, 43, 0, 11, 6, 26, 6345, 145, 784 };

	std::vector<int> answer = del_second_null(elem);

	ASSERT_EQ(answer, control);
}

/// @brief  Проверка, когда <= 1, "0"
TEST(Alg1, NoChange) {
	std::vector<int> elem = { 23, 43, 0, 11, 6, 26, 6345, 145, 784 };
	std::vector<int> control = { 23, 43, 0, 11, 6, 26, 6345, 145, 784 };

	std::vector<int> answer = del_second_null(elem);

	ASSERT_EQ(answer, control);
}



//STL3Alg6°. Даны вектор V и список L; вектор V имеет четное количество элементов. 
//Продублировать последний элемент списка, 
//совпадающий с каким-либо элементом из первой половины исходного вектора. 
//Если список не содержит требуемых элементов, то не изменять его. 
//Использовать алгоритм find_first_of и функцию-член insert для списка.

// TODO: Поправить ссылки



/// @brief Проверка на добавление в конец списка
TEST(Alg6, Add) {
	std::vector<int> vec = { 12, 11, 12, 11, 21, 22, 21, 22 };
	std::list<int> lis = { 33, 3, 2, 1, 21, 0, 11 };
	std::list<int> con = { 33, 3, 2, 1, 21, 0, 11, 11 };

	std::list<int> ans = double_last_elem(vec, lis);

	ASSERT_EQ(ans, con);
}

/// @brief Проверка на отсутствие соответствия
TEST(Alg6, Skip) {
	std::vector<int> vec = { 12, 11, 12, 11, 21, 22, 21, 22 };
	std::list<int> lis = { 33, 3, 2, 1, 21, 0, 1 };
	std::list<int> con = { 33, 3, 2, 1, 21, 0, 1 };

	std::list<int> ans = double_last_elem(vec, lis);

	ASSERT_EQ(ans, con);
}

/// @brief Проверка на чётность вектора
TEST(Alg6, Error) {
	std::vector<int> vec = { 12, 11, 12, 11, 21, 22, 21, };
	std::list<int> lis = { 33, 3, 2, 1, 21, 0, 1 };
	std::list<int> con = {  };

	std::list<int> ans = double_last_elem(vec, lis);

	ASSERT_EQ(ans, con);
}



//STL3Alg14°. Дан вектор V. Обнулить первую пару соседних элементов, 
//имеющих одинаковую четность. Например, список 1, 2, 3, 4, 6, 8, 3, 1 
//должен быть преобразован следующим образом: 1, 2, 3, 0, 0, 8, 3, 1. 
//Если список не содержит соседних элементов с одинаковой четностью, 
//то не изменять его. Использовать алгоритм adjacent_find с параметром — функциональным объектом.

// TODO: Орфографию нейминга поправить. Нейминг сделать выразительнее
// TODO: Перевести на ссылку



/// @brief  Проверка на онулирование
TEST(Alg14, Check) {
	std::vector<int> v = { 1, 2, 3, 5, 6, 7};
	std::vector<int> con = { 1, 2, 0, 0, 6, 7 };

	std::vector<int> ans = resets_the_first_pair(v);

	ASSERT_EQ(ans, con);
}

/// @brief  Приотсутствии соседних элементов, имеющих одинаковую чётность
TEST(Alg14, NoChange) {
	std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
	std::vector<int> con = { 1, 2, 3, 4, 5, 6, 7 };

	std::vector<int> ans = resets_the_first_pair(v);

	ASSERT_EQ(ans, con);
}



//STL3Alg19°. Дано число N (> 0) и дек D. 
//Добавить в начало дека последовательность чисел 1, 2, …, N, 
//а в его конец — последовательность N, N−1, …, 2, 1. 
//Использовать два вызова алгоритма generate_n 
//с одинаковым параметром — функциональным объектом, 
//а также итераторы вставки, возвращаемые функциями front_inserter и back_inserter.

// TODO: Орфографию нейминга поправить. Нейминг сделать выразительнее
// TODO: Ссылки



/// @brief Проверка функции
TEST(Alg19, AddFrontAndBack) {
	int N = 3;
	std::deque<int> d = { 4, 5, 6 };
	std::deque<int> con = { 1, 2, 3, 4, 5, 6, 3, 2, 1 };

	std::deque<int> ans = front_and_back_insert(N, d);

	ASSERT_EQ(ans, con);
}

/// @brief На удовлетворения условия N > 0 
TEST(Alg19, NoChange) {
	int N = 0;
	std::deque<int> d = { 4, 5, 6 };
	std::deque<int> con = { 4, 5, 6 };

	std::deque<int> ans = front_and_back_insert(N, d);

	ASSERT_EQ(ans, con);
}



//STL3Alg21°. Дано число K (0 < K < 10) и списки L1 и L2, 
//каждый из которых содержит не менее 10 элементов. 
//Выполнить для списка L1 циклический сдвиг элементов вправо на K позиций, 
//а для списка L2 — циклический сдвиг влево на K позиций. 
//Использовать алгоритм rotate и функцию advance.

// TODO: Разбить одну функцию на две отдельных




/// @brief Проверка функции 
TEST(Alg21, CheckLeft) {
	int K = 4;
	std::list<int> l1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::list<int> l1_c = { 5, 6, 7, 8, 9, 10, 1, 2, 3, 4 };

	auto ans1 = left_shift(K, l1);

	ASSERT_EQ(ans1, l1_c);
}

/// @brief Проверка на K
TEST(Alg21, NoChangeLeft) {
	int K = 0;
	std::list<int> l1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::list<int> l1_c = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	auto ans1 = left_shift(K, l1);

	ASSERT_EQ(ans1, l1_c);
}




/// @brief Проверка функции 
TEST(Alg21, CheckRight) {
	int K = 4;
	std::list<int> l2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::list<int> l2_c = { 7, 8, 9, 10, 1, 2, 3, 4, 5, 6 };
	auto  ans2 = right_shift(K, l2);

	ASSERT_EQ(ans2, l2_c);
}

/// @brief Проверка на K
TEST(Alg21, NoChangeRight) {
	int K = 0;
	std::list<int> l2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::list<int> l2_c = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	auto ans2 = right_shift(K, l2);

	ASSERT_EQ(ans2, l2_c);
}