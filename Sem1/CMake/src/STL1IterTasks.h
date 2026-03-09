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


/// @brief ������ ���������, � ������� ��������� ��������, ������� �� ������ ������ ������������ ���������� ��� ������ copy_if
/// @param name ���������, ������� ����� �������������
/// @return ��������� � �������, ����������
std::vector<double> even_list_with_copy_if(const std::vector<double>& name);

/// @brief ������ ���������, � ������� ��������� ��������, ������� �� ������ ������ ������������ ���������� ��� ������ ����� for
/// @param name ���������, ������� ����� �������������
/// @return ��������� � �������, ����������
std::vector<double> even_list_with_for(const std::vector<double>& name);

/// @brief ��������� � ����� ������, ������� ��������������� � ��������� � �������
/// @param name �������� ����
/// @return ��������� � ������ �������
std::vector<int> list_from_file(std::istream& name);

/// @brief �������������� ���� name �� �������� �� ����������
/// @param name name2 ���� name ��� ������
/// @param res ��������� ������
void overwriting(std::ostream& name, std::vector<int> res);

/// @brief �������������� � ���� ������ �������� ������� � ������� 0 �� 10
/// @return ��������� ����� �� ���������� ������
std::vector<int> list_with_change(std::istream& num, std::ostream& name);

/// @brief �������������� ���� name2 �� �������� �� ����������
/// @param name2 ���� name2 ��� ������
/// @param res ��������� ������
void record(std::ostream& name2, std::vector<std::string> res);

/// @brief ������ �� ������ ����� ������ �� ����
/// @param name1 ���� name1 ��� ������
/// @param name2 ���� name2 ��� ������
/// @param name3 ���� name2 ��� ������
/// @return ��������� ������ ������ ����
std::vector<std::string> change_file(std::istream& name1, std::istream& words, std::ostream& name2);
