#pragma once

/// @brief Насос и насосная станция: аппроксимация H–Q, перевод коэффициентов в СИ, расчёты PQ/QP/PP.

#include "pipe_oil.h"

#include <vector>
#include <limits>


namespace hydraulics_struct {
    
// ============================================================================
// Свободная функция (вне структур, но внутри пространства имён)
// ============================================================================

/// @brief Перевод вектора коэффициентов для многочлена 3 степени в СИ.
std::vector<double> polynomial_coefficients_si(std::vector<double> polynomial_coefficients);

// =============================================================================
// Насос
// =============================================================================

/// @brief Типы насосов для 3-го порядка
enum class approximation_coefficients_t {
    /// НМ 1250-260
    HM_1250_260,
    /// НМ 5000-210
    HM_5000_210,
    /// НМ 7000-210
    HM_7000_210,
    /// НПВ 1250-60 (1250/1) 
    HPV_1250_60,
    /// ДНм 2500-230 (2500/1)
    DHM_2500_230,
    /// НМ 3600-230
    HM_3600_230,
};

/// @brief Параметры насоса (аппроксимация кривой H–Q, частоты; current — рабочий компромисс).
struct pump_properties_t {
    bool started{true};

    approximation_coefficients_t approximation_coeff;

    /// Частоты насоса
    double nominal_frequency{std::numeric_limits<double>::quiet_NaN()};
    double current_frequency{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Фабричный метод для параметров насоса.
    static pump_properties_t create_pump_properties(const approximation_coefficients_t& approximation_coeff, double nominal_frequency, double current_frequency);

    /// @brief Коэффициенты полинома по возрастанию степени объёмного расхода.
    static std::vector<double> get_polynomial_coefficients(const approximation_coefficients_t& pump_type);
};

// =============================================================================
// Результаты вычислений насоса
// =============================================================================

/// @brief Результат расчёта одного насоса (давления и расход).
struct pump_task_result_t {
    double pressure_in{std::numeric_limits<double>::quiet_NaN()};
    double pressure_out{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
};

// =============================================================================
// Расчёт насоса
// =============================================================================

/// @brief Расчёт напора и давлений одного насоса по кривой и законам подобия частоты.
class pump_calculator_t {
public:
    /// @brief Фиксирует параметры насоса и свойства перекачиваемой жидкости.
    pump_calculator_t(const pump_properties_t& pump_properties, const oil_properties_t& oil)
        // список инициализаторов
        : pump_properties{ pump_properties }
        , oil_properties{ oil }
    {
    // тело конструктора сейчас пустое
    }

private:
    /// @brief Знак для расчёта расхода
    int sign_for_volume_flow(double a, double H, double f) const;

    /// @brief Вычисление полного напора как известного значения черерз давления
    double get_diff_head() const;

    /// @brief Дифнапор на номинальной частоте вращения.
    double get_head_nominal_from_volume_flow(double volume_flow) const;

    /// @brief Дифнапор при current_frequency (законы подобия относительно номинальной).
    double get_head_at_current_frequency(double volume_flow) const;

    /// @brief Расход по напору на номинальной частоте; для кубического полинома — формула Кардано.
    /// Неподдерживаемые случаи — quiet_NaN().
    double get_volume_flow_from_head_nominal() const;

    /// @brief Обратная задача для текущей частоты: напор → объёмный расход.
    double get_volume_flow_from_head_at_current_frequency() const;

public:
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
    double pressure_start{std::numeric_limits<double>::quiet_NaN()};
    double pressure_end{std::numeric_limits<double>::quiet_NaN()};

    /// @brief PQ для насоса
    void solve_pq();

    /// @brief QP для насоса
    void solve_qp();

    /// @brief PP для насоса
    void solve_pp();

    /// @brief Возврат результата
    const pump_task_result_t& get_pump_task_result() const;

private:
    const oil_properties_t oil_properties;
    const pump_properties_t pump_properties;
    pump_task_result_t pump_task_result;
};

// =============================================================================
// Насосная станция
// =============================================================================

/// @brief Параметры насосной станции
struct pump_station_properties_t {
    /// Набор насосов.
    std::vector<pump_properties_t> pumps;

    static pump_station_properties_t create_pump_station(std::vector<pump_properties_t> pumps);

    /// Коэффициенты полинома по возрастанию степени объёмного расхода.
    std::vector<double> get_polynomial_coefficients() const;
};

// =============================================================================
// Результаты вычислений насоса
// =============================================================================

/// @brief Результаты насосной станции
struct pump_station_result_t {
    double pressure_in{std::numeric_limits<double>::quiet_NaN()};
    double pressure_out{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
};

// =============================================================================
// Расчёты насосной станции (последовательное соединение)
// =============================================================================

/// @brief Расчёт последовательно включённых насосов как единой станции.
class pump_station_calculator_t {
public:
    /// @brief Фиксирует состав станции и свойства жидкости.
    pump_station_calculator_t(const pump_station_properties_t pumps, const oil_properties_t& oil)
        // список инициализаторов
        : pump_station_properties{ pumps }
        , oil_properties{ oil }
    {
    // тело конструктора сейчас пустое
    }
private:
    /// @brief Знак для расчёта расхода
    int sign_for_volume_flow(double a, double H, double f) const;

    /// @brief Вычисление полного напора как известного значения черерз давления
    double get_diff_head() const;

    /// @brief Дифнапор на номинальной частоте вращения.
    double get_head_nominal_from_volume_flow(double volume_flow) const;

    /// @brief Расход по напору на номинальной частоте; для кубического полинома — формула Кардано.
    /// Неподдерживаемые случаи — quiet_NaN().
    double get_volume_flow_from_head_nominal() const;

public:
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
    double pressure_start{std::numeric_limits<double>::quiet_NaN()};
    double pressure_end{std::numeric_limits<double>::quiet_NaN()};

    /// @brief PQ для насоса
    void solve_pq();

    /// @brief QP для насоса
    void solve_qp();

    /// @brief PP для насоса
    void solve_pp();

    /// @brief Возврат результата
    const pump_station_result_t& get_pump_station_result() const;

private:
    const pump_station_properties_t pump_station_properties;
    const oil_properties_t oil_properties;
    pump_station_result_t pump_station_result;
};

} //namespace