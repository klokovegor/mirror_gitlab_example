/// @brief Реализация полиномов насоса, станции и постановок PQ/QP.

#include "pump.h"

namespace hydraulics_struct {

// ============================================================================
// Функции для вычисления единиц СИ
// ============================================================================

/// @brief Пересчёт коэффициентов полинома из «часовых» единиц расхода в СИ (м³/с).
std::vector<double> polynomial_coefficients_si(std::vector<double> polynomial_coefficients) {
    if (polynomial_coefficients.size() < 2U) {
        return {};
    }
    std::vector<double> out(4, 0.0);
    out[0] = polynomial_coefficients[0];
    out[1] = polynomial_coefficients[1] * 3600;
    if (polynomial_coefficients.size() > 2U) {
        out[2] = polynomial_coefficients[2] * 3600 * 3600;
    }
    if (polynomial_coefficients.size() > 3U) {
        out[3] = polynomial_coefficients[3] * 3600 * 3600 * 3600;
    }
    return out;
}

// =============================================================================
// pump_properties_t
// =============================================================================

pump_properties_t pump_properties_t::create_pump_properties(const approximation_coefficients_t& approximation_coeff, double nominal_frequency, double current_frequency) {
    pump_properties_t pump_prop;
    pump_prop.approximation_coeff = approximation_coeff;
    pump_prop.nominal_frequency = nominal_frequency;
    pump_prop.current_frequency = current_frequency;
    return pump_prop;
}

std::vector<double> pump_properties_t::get_polynomial_coefficients(const approximation_coefficients_t& pump_type) {
    switch (pump_type) {
        case approximation_coefficients_t::HM_1250_260:
            return {331, 0.451 * 10e-4};
        case approximation_coefficients_t::HM_5000_210:
            return {272, 0.260 * 10e-5};
        case approximation_coefficients_t::HM_7000_210:
            return {299, 0.194 * 10e-5};
        case approximation_coefficients_t::HPV_1250_60:
            return {77.3, 6.7 * 10e-5, 1.1 * 10e-5, 6.3 * 10e-11};
        case approximation_coefficients_t::DHM_2500_230:
            return {253.6, 1.4 * 10e-3, 7.0 * 10e-6, 2 * 10e-10};
        case approximation_coefficients_t::HM_3600_230:
            return {241.2, 4.6 * 10e-4, 7.8 * 10e-6, 1.3 * 10e-11};
    return {};
    }
}

// =============================================================================
// pump_calculator_t
// =============================================================================

int pump_calculator_t::sign_for_volume_flow(double a, double H, double f) const {
    if (std::isnan(a) || std::isnan(H)) {
        throw std::runtime_error("Не задан коэффициент a или полный напор");
    }
    return (a * f * f - H >= 0) ? 1 : -1;
}

double pump_calculator_t::get_diff_head() const {
    if (std::isnan(pressure_end) || std::isnan(pressure_start)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return (pressure_end - pressure_start) / (oil_properties.density * gravity_acceleration);
}

double pump_calculator_t::get_head_nominal_from_volume_flow(double volume_flow) const {
    if (!std::isfinite(volume_flow)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> polynomial_coefficients = pump_properties.get_polynomial_coefficients(pump_properties.approximation_coeff);

    if (polynomial_coefficients.empty()) {
        throw std::runtime_error("Пустой вектор коэффициентов аппроксимации");
    }    

    if (polynomial_coefficients.size() == 2) {
        if (std::isnan(polynomial_coefficients[0]) || std::isnan(polynomial_coefficients[1])) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return polynomial_coefficients[0] - polynomial_coefficients[1] * volume_flow * std::abs(volume_flow);
    }

    if (polynomial_coefficients.size() == 3) {
        throw std::runtime_error("Нахождение полного напора из полинома на данный момент нам не известно");
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double pump_calculator_t::get_head_at_current_frequency(double volume_flow) const {
    if (!std::isfinite(pump_properties.nominal_frequency) || !std::isfinite(pump_properties.current_frequency)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    const double frequency_ratio = pump_properties.current_frequency / pump_properties.nominal_frequency;

    return frequency_ratio * frequency_ratio * get_head_nominal_from_volume_flow(volume_flow * (1 / frequency_ratio));
}

double pump_calculator_t::get_volume_flow_from_head_nominal() const {
    if (!std::isfinite(get_diff_head())) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> polynomial_coefficients = pump_properties.get_polynomial_coefficients(pump_properties.approximation_coeff);

    if (polynomial_coefficients.empty()) {
        throw std::runtime_error("Пустой вектор коэффициентов аппроксимации");
    }    
    if (polynomial_coefficients.size() == 2) {
        if (std::isnan(polynomial_coefficients[0]) || std::isnan(polynomial_coefficients[1])) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return sign_for_volume_flow(polynomial_coefficients[0], get_diff_head(), 1) * std::sqrt(std::abs((polynomial_coefficients[0] - get_diff_head()) / polynomial_coefficients[1]));
    }
    if (polynomial_coefficients.size() == 3) {
        throw std::runtime_error("Нахождение расхода из полинома на данный момент нам не известно");
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double pump_calculator_t::get_volume_flow_from_head_at_current_frequency() const {
    if (!std::isfinite(get_diff_head())) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> polynomial_coefficients = pump_properties.get_polynomial_coefficients(pump_properties.approximation_coeff);
    const double frequency_ratio = pump_properties.current_frequency / pump_properties.nominal_frequency;

    if (polynomial_coefficients.empty()) {
        throw std::runtime_error("Пустой вектор коэффициентов аппроксимации");
    }    
    if (polynomial_coefficients.size() == 2) {
        if (std::isnan(polynomial_coefficients[0]) || std::isnan(polynomial_coefficients[1])) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return sign_for_volume_flow(polynomial_coefficients[0], get_diff_head(), frequency_ratio) * 
                std::sqrt(std::abs((polynomial_coefficients[0] * frequency_ratio * frequency_ratio - get_diff_head()) / polynomial_coefficients[1]));
    }
    if (polynomial_coefficients.size() == 3) {
        throw std::runtime_error("Нахождение расхода из полинома на данный момент нам не известно");
    }

    return std::numeric_limits<double>::quiet_NaN();
}

void pump_calculator_t::solve_pq() {
    double pressure_out = pressure_start + oil_properties.density * gravity_acceleration * get_head_at_current_frequency(volume_flow);
    if (pressure_out < 0) {
        throw std::runtime_error("Отраицательное давление");
    }
    pump_task_result.pressure_out = pressure_out;
}

void pump_calculator_t::solve_qp() {
    double pressure_in = pressure_end - oil_properties.density * gravity_acceleration * get_head_at_current_frequency(volume_flow);
    if (pressure_in < 0) {
        throw std::runtime_error("Отраицательное давление");
    }
    pump_task_result.pressure_in = pressure_in;
}

void pump_calculator_t::solve_pp() {
    throw std::runtime_error("Код пока не реализован");
    /* pump_task_result.volume_flow = get_volume_flow_from_head_at_current_frequency(); */
}

const pump_task_result_t& pump_calculator_t::get_pump_task_result() const {
    if (std::isnan(pump_task_result.pressure_in) && std::isnan(pump_task_result.pressure_out) && std::isnan(pump_task_result.volume_flow)) {
        throw std::runtime_error("Все значения NaN");
    }
    return pump_task_result;
}

// =============================================================================
// pump_station_properties_t
// =============================================================================

pump_station_properties_t pump_station_properties_t::create_pump_station(std::vector<pump_properties_t> pumps) {
    pump_station_properties_t pump_station_properties;
    pump_station_properties.pumps = pumps;
    return pump_station_properties;
}

std::vector<double> pump_station_properties_t::get_polynomial_coefficients() const {
    std::vector<double> out(4, 0.0);
    for (auto& pump: pumps) {
        for (size_t i; i < pump.get_polynomial_coefficients(pump.approximation_coeff).size(); i++) {
            out[i] += pump.get_polynomial_coefficients(pump.approximation_coeff)[i];
        }
    }
    return out;
}

// =============================================================================
// pump_station_calculator_t
// =============================================================================

int pump_station_calculator_t::sign_for_volume_flow(double a, double H, double f) const {
    if (std::isnan(a) || std::isnan(H)) {
        throw std::runtime_error("Не задан коэффициент a или полный напор");
    }
    return (a * f * f - H >= 0) ? 1 : -1;
}

double pump_station_calculator_t::get_diff_head() const {
    if (std::isnan(pressure_end) || std::isnan(pressure_start)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return (pressure_end - pressure_start) / (oil_properties.density * gravity_acceleration);
}

double pump_station_calculator_t::get_head_nominal_from_volume_flow(double volume_flow) const {
    if (!std::isfinite(volume_flow)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> polynomial_coefficients = pump_station_properties.get_polynomial_coefficients();

    if (polynomial_coefficients.empty()) {
        throw std::runtime_error("Пустой вектор коэффициентов аппроксимации");
    }    

    if (polynomial_coefficients.size() == 2) {
        if (std::isnan(polynomial_coefficients[0]) || std::isnan(polynomial_coefficients[1])) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return polynomial_coefficients[0] - polynomial_coefficients[1] * volume_flow * std::abs(volume_flow);
    }

    if (polynomial_coefficients.size() == 3) {
        throw std::runtime_error("Нахождение полного напора из полинома на данный момент нам не известно");
    }

    return std::numeric_limits<double>::quiet_NaN();
}

double pump_station_calculator_t::get_volume_flow_from_head_nominal() const {
    if (!std::isfinite(get_diff_head())) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> polynomial_coefficients = pump_station_properties.get_polynomial_coefficients();

    if (polynomial_coefficients.empty()) {
        throw std::runtime_error("Пустой вектор коэффициентов аппроксимации");
    }    
    if (polynomial_coefficients.size() == 2) {
        if (std::isnan(polynomial_coefficients[0]) || std::isnan(polynomial_coefficients[1])) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return sign_for_volume_flow(polynomial_coefficients[0], get_diff_head(), 1) * std::sqrt(std::abs((polynomial_coefficients[0] - get_diff_head()) / polynomial_coefficients[1]));
    }
    if (polynomial_coefficients.size() == 3) {
        throw std::runtime_error("Нахождение расхода из полинома на данный момент нам не известно");
    }

    return std::numeric_limits<double>::quiet_NaN();
}

void pump_station_calculator_t::solve_pq() {
    double pressure_out = pressure_start + oil_properties.density * gravity_acceleration * get_head_nominal_from_volume_flow(volume_flow);
    if (pressure_out < 0) {
        throw std::runtime_error("Отраицательное давление");
    }
    pump_station_result.pressure_out = pressure_out;
}

void pump_station_calculator_t::solve_qp() {
    double pressure_in = pressure_end - oil_properties.density * gravity_acceleration * get_head_nominal_from_volume_flow(volume_flow);
    if (pressure_in < 0) {
        throw std::runtime_error("Отраицательное давление");
    }
    pump_station_result.pressure_in = pressure_in;
}

void pump_station_calculator_t::solve_pp() {
    throw std::runtime_error("Код пока не реализован");
    /* pump_station_result.volume_flow = get_volume_flow_from_head_nominal(); */
}

const pump_station_result_t& pump_station_calculator_t::get_pump_station_result() const {
    if (std::isnan(pump_station_result.pressure_out) && std::isnan(pump_station_result.volume_flow)) {
        throw std::runtime_error("Все значения NaN");
    }
    return pump_station_result;
}

} //namespace