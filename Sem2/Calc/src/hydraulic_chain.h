#pragma once

/// @brief Гидравлическая цепочка: типы элементов, входные данные, результаты и расчёт последовательности.

#include "pipe_oil.h"
#include "local_resistance.h"
#include "pump.h"

#include <limits>
#include <vector>
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
// Класс для расчётов
// ============================================================================

/// @brief Расчёт гидравлической цепочки в постановках PQ, QP и PP.
class chain_task_calculator_t {
public:
    /// @brief Инициализация калькулятора неизменяемыми свойствами цепочки.
    chain_task_calculator_t(const chain_task_properties_t& chain_prop)
        : chain_task_properties{ chain_prop } 
    {

    }

private:
    /// @brief Проверка наличия данных для расчёта участка трубы.
    void validate_pipe_inputs() const;
    /// @brief Проверка наличия данных для расчёта местного сопротивления.
    void validate_local_resistance_inputs() const;
    /// @brief Проверка наличия данных для расчёта насоса.
    void validate_pump_inputs() const;
    /// @brief Проверка наличия данных для расчёта насосной станции.
    void validate_pump_station_inputs() const;

public:
    double pressure_in{std::numeric_limits<double>::quiet_NaN()};
    double pressure_out{std::numeric_limits<double>::quiet_NaN()};
    double volume_flow{std::numeric_limits<double>::quiet_NaN()};

    /// @brief Постановка PQ: заданы входное давление и расход.
    void solve_pq();
    /// @brief Постановка QP: заданы выходное давление и расход.
    void solve_qp();
    /// @brief Постановка PP: заданы давления на концах.
    void solve_pp();

    /// @brief Доступ к заполненному результату после solve_* (иначе исключение).
    const chain_task_result_t& get_chain_task_result() const;

private:
    const chain_task_properties_t chain_task_properties;
    chain_task_result_t chain_task_result;
};

} // namespace