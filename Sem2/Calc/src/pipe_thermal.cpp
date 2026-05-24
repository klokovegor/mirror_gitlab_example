/// @brief TZ46: реализация теплового прогрева трубопровода.

#include "pipe_thermal.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace thermal_struct {

namespace {

void require_positive(double value, const char* name) {
    if (!(value > 0.0) || std::isnan(value)) {
        throw std::invalid_argument(std::string("pipe_thermal: ") + name + " must be positive");
    }
}

void require_non_negative(double value, const char* name) {
    if (value < 0.0 || std::isnan(value)) {
        throw std::invalid_argument(std::string("pipe_thermal: ") + name + " must be non-negative");
    }
}

/// @brief Число Прандтля Pr = c_p·ν·ρ/λ.
double prandtl_number(const fluid_properties_t& fluid) {
    return fluid.specific_heat * fluid.kinematic_viscosity * fluid.density /
           fluid.thermal_conductivity;
}

} // namespace

void fluid_properties_t::check_parameters() const {
    require_positive(density, "fluid density");
    require_positive(specific_heat, "fluid specific_heat");
    require_positive(thermal_conductivity, "fluid thermal_conductivity");
    require_positive(kinematic_viscosity, "fluid kinematic_viscosity");
    require_positive(volumetric_expansion, "fluid volumetric_expansion");
}

void wall_material_properties_t::check_parameters() const {
    require_positive(density, "wall density");
    require_positive(specific_heat, "wall specific_heat");
    require_positive(thermal_conductivity, "wall thermal_conductivity");
}

pipe_thermal_geometry_t pipe_thermal_geometry_t::create(double inner_diameter_val,
                                                        double outer_diameter_val,
                                                        double length_val) {
    return {inner_diameter_val, outer_diameter_val, length_val};
}

double pipe_thermal_geometry_t::wall_thickness() const {
    return 0.5 * (outer_diameter - inner_diameter);
}

void pipe_thermal_geometry_t::check_parameters() const {
    require_positive(inner_diameter, "inner_diameter");
    require_positive(outer_diameter, "outer_diameter");
    require_positive(length, "length");
    if (outer_diameter <= inner_diameter) {
        throw std::invalid_argument("pipe_thermal: outer_diameter must exceed inner_diameter");
    }
}

void thermal_boundary_t::check_parameters() const {
    require_positive(external_htc, "external_htc");
    if (std::isnan(supply_temperature) || std::isnan(initial_temperature) ||
        std::isnan(ambient_temperature)) {
        throw std::invalid_argument("pipe_thermal: temperatures must be set");
    }
}

double stagnant_conduction_htc_t::alpha_int(double inner_diameter, const fluid_properties_t& fluid,
                                          double /*delta_t_char*/) const {
    require_positive(inner_diameter, "inner_diameter");
    return fluid.thermal_conductivity / inner_diameter; // α_int ≈ λ_f/d_in, неподвижная жидкость
}

natural_convection_htc_t::natural_convection_htc_t(double correlation_c, double correlation_n)
    : correlation_c_(correlation_c), correlation_n_(correlation_n) {
    require_positive(correlation_c, "correlation_c");
    require_positive(correlation_n, "correlation_n");
}

double natural_convection_htc_t::alpha_int(double inner_diameter, const fluid_properties_t& fluid,
                                           double delta_t_char) const {
    require_positive(inner_diameter, "inner_diameter");
    require_positive(delta_t_char, "delta_t_char");

    // Gr = g·β·ΔT·d_in³/ν²
    const double grashof = gravity_acceleration * fluid.volumetric_expansion * delta_t_char *
                           inner_diameter * inner_diameter * inner_diameter /
                           (fluid.kinematic_viscosity * fluid.kinematic_viscosity);
    const double pr = prandtl_number(fluid);
    // Nu = C·(Gr·Pr)^n, затем α_int = Nu·λ_f/d_in
    const double nu =
        correlation_c_ * std::pow(grashof * pr, correlation_n_);
    return nu * fluid.thermal_conductivity / inner_diameter;
}

fixed_alpha_htc_t::fixed_alpha_htc_t(double alpha) : alpha_(alpha) {
    require_positive(alpha_, "alpha");
}

double fixed_alpha_htc_t::alpha_int(double /*inner_diameter*/, const fluid_properties_t& /*fluid*/,
                                    double /*delta_t_char*/) const {
    return alpha_;
}

pipe_thermal_calculator_t::pipe_thermal_calculator_t(
    pipe_thermal_geometry_t geometry, thermal_boundary_t boundary, fluid_properties_t fluid,
    wall_material_properties_t wall, std::unique_ptr<internal_htc_model_t> htc_model)
    : geometry_(std::move(geometry)),
      boundary_(std::move(boundary)),
      fluid_(std::move(fluid)),
      wall_(std::move(wall)),
      htc_model_(std::move(htc_model)) {
    geometry_.check_parameters();
    boundary_.check_parameters();
    fluid_.check_parameters();
    wall_.check_parameters();
    if (!htc_model_) {
        throw std::invalid_argument("pipe_thermal: htc_model is null");
    }
}

thermal_resistance_result_t pipe_thermal_calculator_t::resistances_with_model(
    const internal_htc_model_t& model, double delta_t_char,
    const fluid_properties_t& fluid) const {
    const double d_in = geometry_.inner_diameter;
    const double d_out = geometry_.outer_diameter;
    const double alpha_int = model.alpha_int(d_in, fluid, delta_t_char);

    thermal_resistance_result_t result;
    result.alpha_int = alpha_int;
    result.r_int = 1.0 / (alpha_int * std::numbers::pi * d_in); // R_int, (К·м)/Вт
    result.r_wall =
        std::log(d_out / d_in) / (2.0 * std::numbers::pi * wall_.thermal_conductivity); // R_wall
    result.r_ext = 1.0 / (boundary_.external_htc * std::numbers::pi * d_out); // R_ext
    result.r_total = result.r_int + result.r_wall + result.r_ext;             // R_Σ
    result.biot = boundary_.external_htc * geometry_.wall_thickness() / wall_.thermal_conductivity;
    return result;
}

thermal_resistance_result_t pipe_thermal_calculator_t::compute_resistances_per_meter() const {
    // ΔT_char для α_int: половина перепада T_supply − T_initial
    const double delta_t =
        std::max(boundary_.supply_temperature - boundary_.initial_temperature, 1.0);
    return resistances_with_model(*htc_model_, 0.5 * delta_t, fluid_);
}

thermal_capacity_result_t pipe_thermal_calculator_t::compute_capacity_per_meter() const {
    const double d_in = geometry_.inner_diameter;
    const double d_out = geometry_.outer_diameter;
    const double area_fluid = std::numbers::pi * d_in * d_in / 4.0;                 // площадь жидкости
    const double area_wall = std::numbers::pi * (d_out * d_out - d_in * d_in) / 4.0; // площадь стенки

    thermal_capacity_result_t result;
    // C_th = ρ_f·c_{p,f}·A_f + ρ_w·c_{p,w}·A_w, Дж/(К·м)
    result.capacity_per_meter = fluid_.density * fluid_.specific_heat * area_fluid +
                                wall_.density * wall_.specific_heat * area_wall;
    return result;
}

double pipe_thermal_calculator_t::compute_time_constant() const {
    const auto resistances = compute_resistances_per_meter();
    const auto capacity = compute_capacity_per_meter();
    return resistances.r_total * capacity.capacity_per_meter;
}

thermal_transient_result_t pipe_thermal_calculator_t::time_to_fraction(double eta) const {
    if (eta <= 0.0 || eta >= 1.0 || std::isnan(eta)) {
        throw std::invalid_argument("pipe_thermal: eta must be in (0, 1)");
    }
    thermal_transient_result_t result;
    result.fraction = eta;
    result.time_constant = compute_time_constant();
    result.time_to_fraction = -result.time_constant * std::log(1.0 - eta); // t_η = −τ·ln(1−η)
    return result;
}

convection_comparison_t pipe_thermal_calculator_t::compare_conduction_vs_convection(
    double nu_correlation_c, double nu_correlation_n, double delta_t_char, double eta,
    const fluid_properties_t& fluid_for_convection) const {
    stagnant_conduction_htc_t cond_model;
    natural_convection_htc_t conv_model(nu_correlation_c, nu_correlation_n);

    const auto r_cond = resistances_with_model(cond_model, delta_t_char, fluid_);
    const auto r_conv = resistances_with_model(conv_model, delta_t_char, fluid_for_convection);
    const auto cap = compute_capacity_per_meter();

    convection_comparison_t cmp;
    cmp.conduction.time_constant = r_cond.r_total * cap.capacity_per_meter;
    cmp.convection.time_constant = r_conv.r_total * cap.capacity_per_meter;
    cmp.conduction.fraction = eta;
    cmp.convection.fraction = eta;
    cmp.conduction.time_to_fraction = -cmp.conduction.time_constant * std::log(1.0 - eta);
    cmp.convection.time_to_fraction = -cmp.convection.time_constant * std::log(1.0 - eta);
    if (cmp.conduction.time_constant > 0.0) {
        cmp.tau_ratio = cmp.convection.time_constant / cmp.conduction.time_constant;
    }
    return cmp;
}

circulation_advice_result_t pipe_thermal_calculator_t::circulation_advice() const {
    const double delta_t = boundary_.supply_temperature - boundary_.initial_temperature;
    const double length = geometry_.length;
    circulation_advice_result_t advice;

    const bool long_pipe = length > 100.0;       // критерий task.md: L > 100 м
    const bool large_delta_t = delta_t > 30.0;   // ΔT > 30 K
    const auto transient = time_to_fraction(0.95);
    const bool slow_warmup = transient.time_to_fraction > 3600.0; // прогрев > 1 ч

    if (long_pipe && (large_delta_t || slow_warmup)) {
        advice.recommendation = circulation_recommendation_t::recommended;
        advice.message =
            "Для длинного трубопровода и значительного перепада температур "
            "предварительная циркуляция целесообразна (task.md, этап 6).";
    } else if (length < 20.0 && transient.time_to_fraction < 3600.0) {
        advice.recommendation = circulation_recommendation_t::not_needed;
        advice.message = "Короткий участок: допустим прогрев без принудительной циркуляции.";
    } else {
        advice.recommendation = circulation_recommendation_t::optional;
        advice.message = "Циркуляция может ускорить выравнивание, но не обязательна по критериям ТЗ.";
    }
    return advice;
}

axial_conduction_solver_t::axial_conduction_solver_t(pipe_thermal_geometry_t geometry,
                                                     thermal_boundary_t boundary,
                                                     fluid_properties_t fluid,
                                                     double u_overall_per_meter,
                                                     ftcs_settings_t settings)
    : geometry_(std::move(geometry)),
      boundary_(std::move(boundary)),
      fluid_(std::move(fluid)),
      u_overall_per_meter_(u_overall_per_meter),
      settings_(std::move(settings)) {
    geometry_.check_parameters();
    boundary_.check_parameters();
    fluid_.check_parameters();
    require_positive(u_overall_per_meter_, "u_overall_per_meter");
    if (settings_.nodes < 3) {
        throw std::invalid_argument("pipe_thermal: FTCS requires at least 3 nodes");
    }
}

ftcs_result_t axial_conduction_solver_t::run_until_fraction() const {
    const double length = geometry_.length;
    const std::size_t n = settings_.nodes;
    const double dx = length / static_cast<double>(n - 1); // шаг сетки Δx, м
    const double d_in = geometry_.inner_diameter;
    const double area = std::numbers::pi * d_in * d_in / 4.0; // A, м²
    const double perimeter = std::numbers::pi * d_in;           // P, м

    const double a = fluid_.thermal_conductivity / (fluid_.density * fluid_.specific_heat); // температуропроводность
    const double k_loss = u_overall_per_meter_ * perimeter / (fluid_.density * fluid_.specific_heat * area);
    const double fo = a * settings_.time_step / (dx * dx); // число Фурье Fo = a·Δt/Δx²
    const double beta = k_loss * settings_.time_step;      // параметр потерь β = k·Δt

    if (fo > 0.5 || beta >= 1.0) {
        throw std::invalid_argument("pipe_thermal: FTCS stability violated (Fo <= 0.5, beta < 1)");
    }

    // Целевая T = T_amb + η·(T_supply − T_amb)
    const double t_target =
        boundary_.ambient_temperature +
        settings_.target_fraction * (boundary_.supply_temperature - boundary_.ambient_temperature);

    std::vector<double> temperature(n, boundary_.initial_temperature);
    temperature.front() = boundary_.supply_temperature;

    ftcs_result_t result;
    double time = 0.0;

    while (time < settings_.max_time) {
        std::vector<double> next = temperature;
        next.front() = boundary_.supply_temperature;

        for (std::size_t i = 1; i + 1 < n; ++i) {
            const double laplacian =
                temperature[i - 1] - 2.0 * temperature[i] + temperature[i + 1];
            // T_i^{n+1} = T_i^n + Fo·(T_{i-1}−2T_i+T_{i+1}) − β·(T_i−T_окр)
            next[i] = temperature[i] + fo * laplacian -
                      beta * (temperature[i] - boundary_.ambient_temperature);
        }
        next.back() = temperature[n - 2]; // теплоизоляция на выходе: ∂T/∂x = 0

        temperature = std::move(next);
        time += settings_.time_step;
        ++result.steps;

        double metric = 0.0;
        if (settings_.stop == ftcs_stop_criterion_t::mean_temperature) {
            double sum = 0.0;
            for (double t : temperature) {
                sum += t;
            }
            metric = sum / static_cast<double>(n);
        } else {
            metric = temperature.back();
        }

        if (metric >= t_target) {
            result.reached_target = true;
            result.elapsed_time = time;
            result.final_temperature = metric;
            return result;
        }
    }

    result.elapsed_time = time;
    if (settings_.stop == ftcs_stop_criterion_t::mean_temperature) {
        double sum = 0.0;
        for (double t : temperature) {
            sum += t;
        }
        result.final_temperature = sum / static_cast<double>(n);
    } else {
        result.final_temperature = temperature.back();
    }
    return result;
}

} // namespace thermal_struct
