/// @brief Реализация коэффициентов ξ и решателей PQ/QP для местного сопротивления.

#include "local_resistance.h"

namespace hydraulics_struct {

namespace {

/// @brief Проверка: конечное положительное число.
bool is_finite_positive(double x) {
    return std::isfinite(x) && x > 0.0;
}

}

// =============================================================================
// local_resistance_properties_t
// =============================================================================

local_resistance_properties_t local_resistance_properties_t::create_local_resistance(double user_local_resistance_coefficient, const local_resistance_kind_t& kind, 
                                                            double reference_inner_diameter, double paired_inner_diameter) {
    local_resistance_properties_t local_resistance;
    local_resistance.user_local_resistance_coefficient = user_local_resistance_coefficient;
    local_resistance.kind = kind;
    local_resistance.reference_inner_diameter = reference_inner_diameter;
    local_resistance.paired_inner_diameter = paired_inner_diameter;
    return local_resistance;
}

/// @brief Значения взяты из РД-75.180.00-КТН-198-09 2009
double local_resistance_properties_t::calc_local_resistance_coefficient() const {
    switch (kind) {
        case local_resistance_kind_t::sudden_expansion:
            return local_resistance_coefficient_sudden_expansion(reference_inner_diameter, paired_inner_diameter);
        case local_resistance_kind_t::sudden_narrowing:
            return local_resistance_coefficient_sudden_narrowing(reference_inner_diameter, paired_inner_diameter);
        case local_resistance_kind_t::diaphragm:
            return local_resistance_coefficient_diaphragm(reference_inner_diameter, paired_inner_diameter);
        case local_resistance_kind_t::rotation_pipe_90:
            return 0.5; 
        case local_resistance_kind_t::rotation_pipe_60:
            return 0.4;
        case local_resistance_kind_t::rotation_pipe_45:
            return 0.3;
        case local_resistance_kind_t::rotation_pipe_30:
            return 0.2;
        case local_resistance_kind_t::gate_valve_open:
            return 0.15;
        case local_resistance_kind_t::user_local_resistance_coeff:
            if (std::isnan(user_local_resistance_coefficient)) {
                return std::numeric_limits<double>::quiet_NaN();
            }
            return user_local_resistance_coefficient;
    }
    return std::numeric_limits<double>::quiet_NaN();
}

// =============================================================================
// Местное сопротивление — свободные функции ξ
// =============================================================================

double local_resistance_coefficient_sudden_expansion(double small_inner_diameter, double large_inner_diameter) {
    if (!is_finite_positive(small_inner_diameter) || !is_finite_positive(large_inner_diameter) ||
        small_inner_diameter >= large_inner_diameter) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    const double ratio = small_inner_diameter / large_inner_diameter;
    const double one_minus_area_ratio_sq = 1.0 - ratio * ratio;
    return one_minus_area_ratio_sq * one_minus_area_ratio_sq;
}

double local_resistance_coefficient_sudden_narrowing(double large_inner_diameter, double small_inner_diameter) {
    if (!is_finite_positive(small_inner_diameter) || !is_finite_positive(large_inner_diameter) ||
        small_inner_diameter >= large_inner_diameter) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    const double ratio = small_inner_diameter / large_inner_diameter;
    const double one_minus_area_ratio_sq = 1.0 - ratio * ratio;
    return one_minus_area_ratio_sq * one_minus_area_ratio_sq;
}

double local_resistance_coefficient_diaphragm(double inner_diameter, double diaphragm_inner_diameter) {
    if (!is_finite_positive(inner_diameter) || !is_finite_positive(diaphragm_inner_diameter) ||
        inner_diameter <= diaphragm_inner_diameter) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    const double ratio = diaphragm_inner_diameter / inner_diameter;
    const double epsilon = 0.57 + 0.043 / (1.1 - ratio);
    return std::pow((1 / (ratio * epsilon) - 1), 2);
}

// =============================================================================
// local_resistance_calculator_t
// =============================================================================

int local_resistance_calculator_t::sign_pressure_difference() const {
    if (std::isnan(pressure_start) || std::isnan(pressure_end)) {
        throw std::runtime_error("Не задано начальное или конечное давление");
    }
    return (pressure_start - pressure_end >= 0) ? 1 : -1;
}

double local_resistance_calculator_t::diff_pressure() const {
    return local_resistance_properties.calc_local_resistance_coefficient() * oil_properties.density / 
        (2 * std::pow(pipe_properties.get_pipe_area(), 2)) * volume_flow * std::abs(volume_flow);
}

void local_resistance_calculator_t::solve_pq() {
    double pressure_out = pressure_start - diff_pressure();
    if (pressure_out < 0) {
        throw std::runtime_error("Отраицательное давление");
    }
    local_resistanc_task_result.pressure_out = pressure_out;
}

void local_resistance_calculator_t::solve_qp() {
    double pressure_in = pressure_end + diff_pressure();
    if (pressure_in < 0) {
        throw std::runtime_error("Отраицательное давление");
    }
    local_resistanc_task_result.pressure_in = pressure_in;
}

void local_resistance_calculator_t::solve_pp() {
    throw std::runtime_error("Код пока не реализован");
    /* local_resistanc_task_result.volume_flow = sign_pressure_difference() * std::sqrt((2 * std::pow(pipe_properties.get_pipe_area(), 2) / 
        (oil_properties.density * local_resistance_properties.calc_local_resistance_coefficient()) * 
        std::abs(pressure_start - pressure_end))); */
}

const local_resistanc_task_result_t& local_resistance_calculator_t::get_local_resistanc_task_result() const {
    if (std::isnan(local_resistanc_task_result.pressure_in) && std::isnan(local_resistanc_task_result.pressure_out) && std::isnan(local_resistanc_task_result.volume_flow)) {
        throw std::runtime_error("Все значения NaN");
    }
    return local_resistanc_task_result;
}

} //namespace