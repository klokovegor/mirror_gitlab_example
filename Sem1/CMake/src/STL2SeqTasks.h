#pragma once

#include <iterator>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>
#include <deque>

/// @brief ��������� � ����� ������, ������� ��������������� � ������ � �������
/// @param name �������� ����
/// @return ������ � ������ �������
std::vector<int> file_from_vector(std::istream& name);

/// @brief ��������� �� ���� � ������ �� ����������� �����
/// @param d ���
/// @param l ������
/// @return ���������� ������
std::list<int> add_dec_in_list(std::deque<int>& d, std::list<int> l);

/// @brief ��������� ����� ������� �������� ������� -1
/// @param v ����������� ������
/// @return ���������� ������
std::vector<int> add_one_in_vector(std::vector<int>& v);

/// @brief ������� �������� ��������� ������� � �������� ���������� ��������
/// @param v �������� ������
/// @return ���������� ������
std::vector<int> delele_uneven_value(std::vector<int>& v);

/// @brief ������� � ������ �������� ��������� ������ ��� �������� � ��������� ����������� ��������
/// @param l �������� ������
/// @return ���������� ������
std::list<int> del_part_of_list(std::list<int> l);

