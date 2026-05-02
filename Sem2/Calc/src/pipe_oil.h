#pragma once

/// @brief Трубопровод и нефть: единицы СИ, геометрия, потери напора и экспорт профилей в CSV.

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

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
// Профиль трубы (координаты и место сохранения файла)
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
    double get_length(size_t index) const;

    /// @brief Проверка данных структуры
    void check_parameters() const;
};

// =============================================================================
// Параметры трубы
// =============================================================================

/// @brief Геометрия трубы: внутренний диаметр, толщина стенки, шероховатость
struct pipe_properties_t {
    pipe_profile_t pipe_profile;

    double inner_diameter{std::numeric_limits<double>::quiet_NaN()};
    double wall_thickness{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Фабричный метод для создания стандартной трубы
    static pipe_properties_t create_pipe(double inner_diameter_val, double wall_thickness_val);

    /// @brief Относительная шероховатость
    double get_relative_roughness() const;

    /// @brief Внешний диаметр
    double get_outer_diameter() const;

    /// @brief Площадь сечения
    double get_pipe_area() const;

    /// @brief Проверка данных структуры
    void check_parameters() const;
};

// =============================================================================
// Свойства вещества
// =============================================================================

/// @brief Свойства жидкости: плотность и вязкость
struct oil_properties_t {
    double density{std::numeric_limits<double>::quiet_NaN()};
    double kinematic_viscosity{std::numeric_limits<double>::quiet_NaN()};
    double dynamic_viscosity{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Фабричный метод: плотность (кг/м³), кинематическая вязкость (м²/с), динамическая (Па·с)
    static oil_properties_t create_oil(double density_val, double kinematic_viscosity_val);

    /// @brief Вычисления кинематической вязкости из динамической
    static double calc_kinematic_by_dynamic(double density, double dynamic_viscosity);

    /// @brief Проверка данных структуры
    void check_parameters() const;
};

// =============================================================================
// Результаты вычислений трубы
// =============================================================================

/// @brief Результат расчёта: профиль давления, профиль полного напора
struct pipe_task_result_t {
    std::vector<double> head_profile;
    std::vector<double> pressure_profile;
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
};

// ============================================================================
// Расчёта трубы
// ============================================================================

class pipe_calculator_t {
public:
    pipe_calculator_t(const pipe_profile_t& profile, const pipe_properties_t& pipe, const oil_properties_t& oil)
        // список инициализаторов
        : profile{profile}
        , pipe_properties{ pipe }
        , oil_properties{ oil }
    {
    // тело конструктора сейчас пустое
    }

private:
    /// @brief Скорость
    double get_velocity() const;

    /// @brief Число Рейнольдса
    double get_reynolds_number() const;

    /// @brief Полный напор в конкретной точке
    double get_total_head(size_t index, double pressure) const;

    /// @brief Знак разности напоров
    int sign_head_difference() const;

    /// @brief Коэффициент гидравлического сопротивления
    double get_hydraulic_resistance_coefficient() const;

    /// @brief Объёмный расход
    double get_volume_flow_rate() const;

    /// @brief Разница геометрического напора на участке
    double get_diff_elevation(size_t index) const;

    /// @brief Потеря полного напора
    double get_diff_total_head(size_t index) const;

    /// @brief Потеря давленмя на учасике трубы
    double get_diff_pressure(size_t index) const;

    /// @brief Скорость в PP задаче
    double get_velocity_by_solve_pp() const;

public:
    double mass_flow{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
    double pressure_start{std::numeric_limits<double>::quiet_NaN()};
    double pressure_end{std::numeric_limits<double>::quiet_NaN()};
    double elevation_start{std::numeric_limits<double>::quiet_NaN()};
    double elevation_end{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Решить задачу №1: найти выходное давление при известном массовом расходе
    void solve_pq();

    /// @brief Решить задачу №1: найти входное давление при известном массовом расходе
    void solve_qp();

    /// @brief Решить задачу №2: найти массовый расход при известном перепаде давлений
    void solve_pp();

    const pipe_task_result_t& get_pipe_task_result() const;

private:
    const pipe_profile_t profile;
    const pipe_properties_t pipe_properties;
    const oil_properties_t oil_properties;
    pipe_task_result_t pipe_task_result;
};

// ============================================================================
// Сохранение профилей в формате CSV
// ============================================================================

/// @brief Записать профиль полного напора в CSV (формат: x;z;H)
bool write_head_profile_csv(std::string filepath);

/// @brief Записать профиль давления в CSV (формат: x;z;p)
bool write_pressure_profile_csv(std::string filepath);

} //namespace