#pragma once

#include "gtest/gtest.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

/// @brief Перевести длину трубопровода из километров в метры.
/// @param length_km Длина в километрах.
/// @return Длина в метрах.
double length_si(double length_km);

/// @brief Перевести внутренний диаметр трубопровода из миллиметров в метры.
/// @param diameter_mm Внутренний диаметр в миллиметрах.
/// @return Внутренний диаметр в метрах.
double diameter_si(double diameter_mm);

/// @brief Перевести давление в Паскали (Па) в зависимости от указанных единиц.
/// @param pressure_value Значение давления в указанных единицах.
/// @param unit Единицы измерения: поддерживаются строки `"кПа"` и `"кгс/см^2"`.
///             Если unit не распознана, функция возвращает исходное `pressure_value` как Па.
/// @return Давление в Паскалях.
double pressure_si(double pressure_value, std::string unit);

/// @brief Перевести динамическую вязкость из сП в Па·с.
/// @param dynamic_viscosity_cp Динамическая вязкость в сантипуазах (сП).
/// @return Динамическая вязкость в Па·с.
double dynamic_viscosity_si(double dynamic_viscosity_cp);

/// @brief Перевести кинематическую вязкость из сСт в м^2/с.
/// @param kinematic_viscosity_cst Кинематическая вязкость в сантистоксах (сСт).
/// @return Кинематическая вязкость в м^2/с.
double kinematic_viscosity_si(double kinematic_viscosity_cst);

/// @brief Посчитать число Рейнольдса для внутреннего потока.
/// @param velocity_m_s Средняя скорость потока, м/с.
/// @param diameter_m Внутренний диаметр, м.
/// @param kinematic_viscosity_m2_s Кинематическая вязкость, м^2/с.
/// @return Число Рейнольдса `Re`.
double reynolds_number_calc(double velocity_m_s, double diameter_m, double kinematic_viscosity_m2_s);

/// @brief Посчитать полный напор (в форме удельной механической энергии).
/// @param pressure_pa Статическое давление, Па.
/// @param density_kg_m3 Плотность жидкости, кг/м^3.
/// @param elevation_m Геодезическая отметка (высота), м.
/// @return Полный напор `H` в метрах.
double total_head_calc(double pressure_pa, double density_kg_m3, double elevation_m);

/// @brief Вернуть знак разности двух чисел.
/// @param a Первое число.
/// @param b Второе число.
/// @return `1`, если `a > b`, иначе `-1`.
int sign(double a, double b);

/// @brief Посчитать относительную шероховатость.
/// @param equivalent_roughness_m Эквивалентная шероховатость, м.
/// @param diameter_m Внутренний диаметр, м.
/// @return Относительная шероховатость `epsilon = delta / d`.
double epsilon(double equivalent_roughness_m, double diameter_m);

/// @brief Посчитать коэффициент гидравлического сопротивления (в упрощённой модели).
/// @param reynolds_number Число Рейнольдса `Re`.
/// @param relative_roughness Относительная шероховатость `epsilon`.
/// @return Коэффициент трения `lambda`.
double lambda_calc(double reynolds_number, double relative_roughness);

/// @brief Посчитать площадь внутреннего сечения трубопровода.
/// @param diameter_m Внутренний диаметр, м.
/// @return Площадь `S` в м^2.
double pipe_area_calc(double diameter_m);

/// @brief Посчитать кинематическую вязкость по динамической и плотности.
/// @param dynamic_viscosity_pa_s Динамическая вязкость, Па·с.
/// @param density_kg_m3 Плотность жидкости, кг/м^3.
/// @return Кинематическая вязкость, м^2/с.
double kinematic_viscosity_calc(double dynamic_viscosity_pa_s, double density_kg_m3);

/// @brief Посчитать объёмный расход по массовому расходу и плотности.
/// @param mass_flow_kg_s Массовый расход, кг/с.
/// @param density_kg_m3 Плотность жидкости, кг/м^3.
/// @return Объёмный расход `Q`, м^3/с.
double q_calc(double mass_flow_kg_s, double density_kg_m3);

/// @brief Решить задачу №1: найти входное давление при известном массовом расходе.
/// @param length_m Длина трубопровода, м.
/// @param diameter_m Внутренний диаметр, м.
/// @param elevation_start_m Начальная отметка (хранилище), м.
/// @param elevation_end_m Конечная отметка (реактор), м.
/// @param pressure_end_pa Давление на выходе (в точке конца), Па.
/// @param mass_flow_kg_s Массовый расход, кг/с.
/// @param density_kg_m3 Плотность, кг/м^3.
/// @param dynamic_viscosity_pa_s Динамическая вязкость, Па·с.
/// @return Давление на входе, Па.
double qp_task(double length_m, double diameter_m, double elevation_start_m, double elevation_end_m, double pressure_end_pa,
                double mass_flow_kg_s, double density_kg_m3, double dynamic_viscosity_pa_s);

/// @brief Решить задачу №2: найти массовый расход при известном перепаде давлений.
/// @param length_m Длина трубопровода, м.
/// @param diameter_m Внутренний диаметр, м.
/// @param elevation_start_m Начальная отметка (резервуар), м.
/// @param elevation_end_m Конечная отметка (эстакада), м.
/// @param pressure_start_pa Давление на входе, Па.
/// @param pressure_end_pa Давление на выходе, Па.
/// @param density_kg_m3 Плотность, кг/м^3.
/// @param kinematic_viscosity_m2_s Кинематическая вязкость, м^2/с.
/// @return Массовый расход, кг/с.
double pp_task(double length_m, double diameter_m, double elevation_start_m, double elevation_end_m, double pressure_start_pa,
                double pressure_end_pa, double density_kg_m3, double kinematic_viscosity_m2_s);

/// @brief Построить профиль статического давления вдоль длины (линейная интерполяция).
/// @param pressure_start_pa Давление на входе, Па.
/// @param pressure_end_pa Давление на выходе, Па.
/// @param length_m Длина трубопровода, м.
/// @param step_intervals Количество интервалов разбиения по длине (будет `step_intervals + 1` точек).
/// @return Вектор точек, где каждая точка — пара `{x, P}` (внутренний вектор размера 2).
std::vector<std::vector<double>> pressure_profile(double pressure_start_pa, double pressure_end_pa, double length_m, int step_intervals);

/// @brief Записать профиль давления в CSV.
/// Формат: две строки (каждая строка — серия значений через `;`):
/// первая строка: `x;...`, вторая строка: `P;...`.
/// @param filepath Путь к CSV-файлу.
/// @param pressure_prof Профиль точек `{x, P}`.
/// @param step_intervals Количество интервалов (ожидается согласованность с размером профиля).
/// @return `true`, если файл успешно открыт и запись выполнена.
bool write_pressure_profile_csv(const std::string& filepath, const std::vector<std::vector<double>>& pressure_prof, int step_intervals);

/// @brief Построить профиль полного напора вдоль длины на основе профиля давления.
/// @param pressure_start_pa Давление на входе, Па.
/// @param pressure_end_pa Давление на выходе, Па.
/// @param length_m Длина трубопровода, м.
/// @param step_intervals Количество интервалов разбиения по длине.
/// @param density_kg_m3 Плотность, кг/м^3.
/// @param elevation_start_m Начальная отметка, м.
/// @param elevation_end_m Конечная отметка, м.
/// @return Вектор точек `{x, H}` (внутренний вектор размера 2).
std::vector<std::vector<double>> head_profile(double pressure_start_pa, double pressure_end_pa, double length_m, int step_intervals,
                                                double density_kg_m3, double elevation_start_m, double elevation_end_m);

/// @brief Записать профиль полного напора в CSV.
/// Формат: две строки (каждая строка — серия значений через `;`):
/// первая строка: `x;...`, вторая строка: `H;...`.
/// @param filepath Путь к CSV-файлу.
/// @param head_prof Профиль точек `{x, H}`.
/// @param step_intervals Количество интервалов.
/// @return `true`, если файл успешно открыт и запись выполнена.
bool write_head_profile_csv(const std::string& filepath, const std::vector<std::vector<double>>& head_prof, int step_intervals);
