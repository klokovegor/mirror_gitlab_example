#pragma once

/// @brief Гидравлическая цепочка: базовый интерфейс элементов, входные данные, результаты и расчёт.

#include <array>
#include <limits>
#include <memory>
#include <vector>

namespace hydraulics_struct {

struct chain_task_result_t;

/// @brief Базовый полиморфный элемент гидравлической цепочки (lab10).
class hydro_model_t {
public:
    virtual ~hydro_model_t() = default;

    virtual void solve_pq() = 0;
    virtual void solve_qp() = 0;
    virtual void solve_pp() = 0;

    virtual void apply_pq_boundary(double pressure_in, double volume_flow) = 0;
    virtual void apply_qp_boundary(double pressure_out, double volume_flow) = 0;
    virtual void apply_pp_boundary(double pressure_in, double pressure_out) = 0;

    virtual double outlet_pressure_after_pq() const = 0;
    virtual double inlet_pressure_after_qp() const = 0;
    virtual double volume_flow_after_pp() const = 0;

    virtual void commit_pq_result(chain_task_result_t& chain_result) const = 0;
    virtual void commit_qp_result(chain_task_result_t& chain_result) const = 0;
    virtual void commit_pp_result(chain_task_result_t& chain_result) const = 0;

    /// @brief Начальное приближение Q для PP; по умолчанию не задано.
    virtual double suggest_pp_initial_volume_flow() const;
};

} // namespace hydraulics_struct

#include "pipe_oil.h"
#include "local_resistance.h"
#include "pump.h"

#include <ranges>

namespace hydraulics_struct {

// ============================================================================
// Структура для входных данных
// ============================================================================

/// @brief Тип элемента в последовательности гидравлической цепочки.
enum class type_of_obj_t {
    pipe,
    local_resistance,
    pump,
    pump_station,
};

/// @brief Входные данные расчёта цепочки: профили, свойства жидкости и порядок элементов.
struct chain_task_properties_t {
    std::vector<pipe_profile_t> pipe_profile;
    std::vector<pipe_properties_t> pipe_properties;
    oil_properties_t oil_properties;
    std::vector<local_resistance_properties_t> local_resistance_properties;
    std::vector<pump_properties_t> pump_properties;
    std::vector<pump_station_properties_t> pump_station_properties;

    std::vector<type_of_obj_t> chain;

    /// @brief Создать набор свойств с заданным порядком типов элементов цепочки.
    static chain_task_properties_t create_chain(std::vector<type_of_obj_t> chain);
};

// ============================================================================
// Структура для результатов
// ============================================================================

/// @brief Агрегированный результат расчёта по элементам цепочки и суммарные величины.
struct chain_task_result_t {
    pipe_task_result_t pipe_task_result;
    local_resistanc_task_result_t local_resistanc_task_result;
    pump_task_result_t pump_task_result;
    pump_station_result_t pump_station_result;

    double pressure_in{std::numeric_limits<double>::quiet_NaN()};
    double pressure_out{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};
};

// ============================================================================
// Композит: цепочка элементов
// ============================================================================

/// @brief Полиморфная гидравлическая цепочка (PQ / QP / PP).
class hydro_chain_model_t : public hydro_model_t {
public:
    explicit hydro_chain_model_t(const chain_task_properties_t& chain_prop);

    double pressure_in{std::numeric_limits<double>::quiet_NaN()};
    double pressure_out{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};

    void solve_pq() override;
    void solve_qp() override;
    void solve_pp() override;

    void apply_pq_boundary(double pressure_in_val, double volume_flow_val) override;
    void apply_qp_boundary(double pressure_out_val, double volume_flow_val) override;
    void apply_pp_boundary(double pressure_in_val, double pressure_out_val) override;

    double outlet_pressure_after_pq() const override;
    double inlet_pressure_after_qp() const override;
    double volume_flow_after_pp() const override;

    void commit_pq_result(chain_task_result_t& chain_result) const override;
    void commit_qp_result(chain_task_result_t& chain_result) const override;
    void commit_pp_result(chain_task_result_t& chain_result) const override;

    /// @brief Доступ к заполненному результату после solve_* (иначе исключение).
    const chain_task_result_t& get_chain_task_result() const;

private:
    struct pp_solver_settings_t {
        double residual_tolerance{1e-8};
        double argument_tolerance{1e-8};
        size_t max_iterations{100};
        double flow_zero_tolerance{1e-12};
        double fallback_initial_volume_flow{0.01};
    };

    void ensure_chain_built();
    void build_chain_models();
    static void ensure_not_null(const hydro_model_t* elem);

    double outlet_pressure_for_pq_flow(double q_trial) const;
    double pp_residual(double q_trial) const;
    double suggest_pp_initial_volume_flow_for_chain() const;
    void validate_pp_boundaries() const;

    const chain_task_properties_t chain_task_properties;
    chain_task_result_t chain_task_result;
    std::array<bool, 4> chain_has_type_{};
    pp_solver_settings_t pp_solver_settings_;
    std::vector<std::unique_ptr<hydro_model_t>> owned_models_;
    std::vector<hydro_model_t*> chain_models_;
};

} // namespace
