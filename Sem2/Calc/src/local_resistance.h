#pragma once

/// @brief Местные сопротивления: коэффициенты ξ, свойства и калькулятор PQ/QP/PP.

#include "pipe_oil.h"

#include <limits>

namespace hydraulics_struct {

// =============================================================================
// Местное сопротивление
// =============================================================================

/// @brief Вид местного сопротивления.
enum class local_resistance_kind_t {
    /// Внезапное расширение.
    sudden_expansion,
    /// Внезапное сужение.
    sudden_narrowing,
    /// Диафрагма.
    diaphragm,
    /// Бесшовный поворот трубы на 90 градусов
    rotation_pipe_90,
    /// Бесшовный поворот трубы на 60 градусов
    rotation_pipe_60,
    /// Бесшовный поворот трубы на 45 градусов
    rotation_pipe_45,
    /// Бесшовный поворот трубы на 30 градусов
    rotation_pipe_30,
    /// Открытая задвижка  — постоянный ξ (или из user_local_resistance_coefficient).
    gate_valve_open,
    /// Пользовательское значение
    user_local_resistance_coeff,
};

/// @brief Параметры местного сопротивления (геометрия арматуры / перехода).
struct local_resistance_properties_t {
    /// Явный коэффициент ξ для константных видов.
    double user_local_resistance_coefficient{std::numeric_limits<double>::quiet_NaN()};
    
    /// Вид сопротивления.
    local_resistance_kind_t kind{local_resistance_kind_t::user_local_resistance_coeff};

    /// Диаметр участка.
    double reference_inner_diameter{std::numeric_limits<double>::quiet_NaN()};

    /// Второй диаметр для внезапного расширения/сужения и диафрагмы.
    double paired_inner_diameter{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Фабричный метод для местных сопротивлений
    static local_resistance_properties_t create_local_resistance(double user_local_resistance_coefficient, const local_resistance_kind_t& kind, 
                                                double reference_inner_diameter, double paired_inner_diameter);

    /// @brief Коэффициент местного сопротивления ξ.
    double calc_local_resistance_coefficient() const;
};

/// @brief ξ внезапного расширения.
double local_resistance_coefficient_sudden_expansion(double small_inner_diameter, double large_inner_diameter);

/// @brief ξ внезапного сужение.
double local_resistance_coefficient_sudden_narrowing(double large_inner_diameter, double small_inner_diameter);

/// @brief ξ диафрагмы.
double local_resistance_coefficient_diaphragm(double inner_diameter, double diaphragm_inner_diameter);

// =============================================================================
// Результаты вычислений местного сопротивления
// =============================================================================

/// @brief Результат расчёта местного сопротивления (давления и расход).
struct local_resistanc_task_result_t {
    double pressure_in{std::numeric_limits<double>::quiet_NaN()};
    double pressure_out{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
};

// =============================================================================
// Расчёт местых сопротивлений
// =============================================================================

/// @brief Гидравлический расчёт единичного местного сопротивления на участке заданной трубы.
class local_resistance_calculator_t : public hydro_model_t {
public:
    /// @brief Связывает геометрию трубы, свойства нефти и параметры арматуры.
    local_resistance_calculator_t(const pipe_properties_t& pipe_properties, const oil_properties_t& oil_properties, const local_resistance_properties_t& local_resistance_properties)
        // список инициализаторов
        : pipe_properties{ pipe_properties }
        , oil_properties{ oil_properties }
        , local_resistance_properties{ local_resistance_properties }
    {
    // тело конструктора сейчас пустое
    }

private:
    /// @brief Знак разности давлений
    int sign_pressure_difference() const;

    /// @brief Разность давлений
    double diff_pressure() const;

public:
    double pressure_start{std::numeric_limits<double>::quiet_NaN()};
    double pressure_end{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};

    void solve_pq() override;
    void solve_qp() override;
    void solve_pp() override;

    void apply_pq_boundary(double pressure_in, double volume_flow) override;
    void apply_qp_boundary(double pressure_out, double volume_flow) override;
    void apply_pp_boundary(double pressure_in, double pressure_out) override;

    double outlet_pressure_after_pq() const override;
    double inlet_pressure_after_qp() const override;
    double volume_flow_after_pp() const override;

    void commit_pq_result(chain_task_result_t& chain_result) const override;
    void commit_qp_result(chain_task_result_t& chain_result) const override;
    void commit_pp_result(chain_task_result_t& chain_result) const override;

    const local_resistanc_task_result_t& get_local_resistanc_task_result() const;

private:
    const pipe_properties_t pipe_properties;
    const oil_properties_t oil_properties;
    const local_resistance_properties_t local_resistance_properties;
    local_resistanc_task_result_t local_resistanc_task_result;
};

} //namespace