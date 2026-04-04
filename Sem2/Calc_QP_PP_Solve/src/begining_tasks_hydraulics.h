#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <map>
#include <functional>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>


namespace hydraulics_struct {

/// @brief Ускорение свободного падения Земли
constexpr double gravity_acceleration = 9.81;

/// @brief Эквивалентная шероховатость (принятая константа модели)
constexpr double equivalent_roughness = 1e-4;

// ============================================================================
// Свободные функции (вне структур, но внутри пространства имён)
// ============================================================================

/// @brief Перевести длину трубопровода из километров в метры.
double length_si(double length, const std::string& unit = "м");

/// @brief Перевести внутренний диаметр трубопровода из миллиметров в метры.
double diameter_si(double diameter, const std::string& unit = "м");

/// @brief Перевести давление в Паскали (Па) в зависимости от указанных единиц.
double pressure_si(double pressure_value, const std::string& unit = "Па");

/// @brief Перевести кинематическую вязкость из сСт в м^2/с.
double kinematic_viscosity_si(double kinematic_viscosity, const std::string& unit = "м^2/с");

/// @brief Перевести динамическую вязкость из сП в Па·с.
double dynamic_viscosity_si(double dynamic_viscosity, const std::string& unit = "Па·с");

// ============================================================================
// Структуры предметной области
// ============================================================================

/// @brief Координаты точек вдоль трубы и соответствующие высоты
struct pipe_profile_t {
    std::vector<double> coordinates;
    std::vector<double> elevations;

    /// @brief Фабричный метод для создания профиля точки
    static pipe_profile_t create_profile(std::vector<double> x, std::vector<double> z);

    /// @brief Количество точек в профиле (для коллекции)
    size_t get_point_count() const;

    /// @brief Длина профиля (для коллекции)
    double get_length() const;
};

/// @brief Геометрия трубы: внутренний диаметр, толщина стенки, шероховатость
struct pipe_properties_t {
    double length{std::numeric_limits<double>::quiet_NaN()};
    double inner_diameter{std::numeric_limits<double>::quiet_NaN()};
    double wall_thickness{std::numeric_limits<double>::quiet_NaN()};
    double start_elevation{std::numeric_limits<double>::quiet_NaN()};
    double end_elevation{std::numeric_limits<double>::quiet_NaN()};
    double mass_flow{std::numeric_limits<double>::quiet_NaN()};
    double pressure_start{std::numeric_limits<double>::quiet_NaN()};
    double pressure_end{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Фабричный метод для создания стандартной трубы
    static pipe_properties_t create_pipe(double length_val, double inner_diameter_val, double wall_thickness_val,
                                            double elevation_start_val, double elevation_end_val);

    /// @brief Относительная шероховатость
    double get_relative_roughness() const;

    /// @brief Внешний диаметр
    double get_outer_diameter() const;

    /// @brief Площадь сечения
    double get_pipe_area() const;
};

/// @brief Свойства жидкости: плотность и вязкость
struct oil_properties_t {
    double density{std::numeric_limits<double>::quiet_NaN()};
    double kinematic_viscosity{get_kinematic_viscosity()};
    double dynamic_viscosity{get_dynamic_viscosity()};

    /// @brief Фабричный метод: плотность (кг/м³), кинематическая вязкость (м²/с), динамическая (Па·с)
    static oil_properties_t create_oil(double density_val, double kinematic_viscosity_val,
                                            double dynamic_viscosity_val);
        
    /// @brief Кинематическая вязкость как const метод
    double get_kinematic_viscosity() const;

    /// @brief Динамическая вязкость как const метод
    double get_dynamic_viscosity() const;
};

/// @brief Результат расчёта: массовый расход, профиль давления, профиль полного напора
struct pipe_task_result_t {
    pipe_properties_t pipe_properties;
    oil_properties_t oil_properties;

    /// @brief Число Рейнольдса
    double get_reynolds_number() const;

    /// @brief Начальный полный напор
    double get_start_total_head() const;

    /// @brief Конечный полный напор
    double get_end_total_head() const;

    /// @brief Знак разности напоров
    int sign_difference() const;

    /// @brief Коэффициент гидравлического сопротивления
    double get_hydraulic_resistance_coefficient() const;

    /// @brief Объёмный расход
    double get_volume_flow_rate() const;

    /// @brief Скорость
    double get_velocity() const;
};

/// @brief Расчётная структура
struct pipe_calculator_t {
    pipe_properties_t pipe_properties;
    oil_properties_t oil_properties;
    pipe_task_result_t pipe_task_result;

    /// @brief Решить задачу №1: найти входное давление при известном массовом расходе
    double solve_pq() const;

    /// @brief Решить задачу №2: найти массовый расход при известном перепаде давлений
    double solve_pp() const;
};

// ============================================================================
// Функции для работы с профилями
// ============================================================================

/// @brief Построить профиль полного напора на основе профиля давления
/// @return вектор точек {x, z, H}
std::vector<std::vector<double>> head_profile(
    pipe_calculator_t& calculator,
    pipe_profile_t& profile);

/// @brief Записать профиль полного напора в CSV (формат: x;z;H)
bool write_head_profile_csv(
    const std::string& filepath, 
    const std::vector<std::vector<double>>& head_prof);

/// @brief Построить профиль статического давления вдоль длины
/// @return вектор точек {x, z, p}
std::vector<std::vector<double>> pressure_profile(
    pipe_calculator_t& calculator,
    pipe_profile_t& profile);

/// @brief Записать профиль давления в CSV (формат: x;z;p)
bool write_pressure_profile_csv(
    const std::string& filepath, 
    const std::vector<std::vector<double>>& pressure_prof);
}