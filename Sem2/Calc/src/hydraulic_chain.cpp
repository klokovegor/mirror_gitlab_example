/// @brief Реализация полиморфной цепочки: сборка элементов и solve_pq / solve_qp / solve_pp.

#include "hydraulic_chain.h"

#include <cmath>
#include <fixed/fixed.h>
#include <stdexcept>

namespace hydraulics_struct {

namespace {

struct chain_build_indices_t {
    size_t pipe_index{0};
    size_t local_resist_index{0};
    size_t pump_index{0};
    size_t pump_station_index{0};
};

void validate_pipe_inputs(const chain_task_properties_t& props) {
    if (props.pipe_properties.empty()) {
        throw std::runtime_error("Для расчета трубы требуется pipe_properties");
    }
    if (props.pipe_profile.empty()) {
        throw std::runtime_error("Для расчета трубы требуется pipe_profile");
    }
}

void validate_local_resistance_inputs(const chain_task_properties_t& props) {
    if (props.local_resistance_properties.empty()) {
        throw std::runtime_error("Для расчета местного сопротивления требуется local_resistance_properties");
    }
    if (props.pipe_properties.empty()) {
        throw std::runtime_error("Для расчета местного сопротивления требуется pipe_properties");
    }
}

void validate_pump_inputs(const chain_task_properties_t& props) {
    if (props.pump_properties.empty()) {
        throw std::runtime_error("Для расчета насоса требуется pump_properties");
    }
}

void validate_pump_station_inputs(const chain_task_properties_t& props) {
    if (props.pump_station_properties.empty()) {
        throw std::runtime_error("Для расчета насосной станции требуется pump_station_properties");
    }
}

std::unique_ptr<hydro_model_t> create_pipe_model(
    const chain_task_properties_t& props,
    chain_build_indices_t& indices)
{
    validate_pipe_inputs(props);
    auto model = std::make_unique<pipe_calculator_t>(
        props.pipe_profile[indices.pipe_index],
        props.pipe_properties[indices.pipe_index],
        props.oil_properties);
    ++indices.pipe_index;
    return model;
}

std::unique_ptr<hydro_model_t> create_local_resistance_model(
    const chain_task_properties_t& props,
    chain_build_indices_t& indices)
{
    validate_local_resistance_inputs(props);
    const size_t pipe_ref_index = indices.pipe_index;
    auto model = std::make_unique<local_resistance_calculator_t>(
        props.pipe_properties[pipe_ref_index],
        props.oil_properties,
        props.local_resistance_properties[indices.local_resist_index]);
    ++indices.local_resist_index;
    return model;
}

std::unique_ptr<hydro_model_t> create_pump_model(
    const chain_task_properties_t& props,
    chain_build_indices_t& indices)
{
    validate_pump_inputs(props);
    auto model = std::make_unique<pump_calculator_t>(
        props.pump_properties[indices.pump_index],
        props.oil_properties);
    ++indices.pump_index;
    return model;
}

std::unique_ptr<hydro_model_t> create_pump_station_model(
    const chain_task_properties_t& props,
    chain_build_indices_t& indices)
{
    validate_pump_station_inputs(props);
    auto model = std::make_unique<pump_station_calculator_t>(
        props.pump_station_properties[indices.pump_station_index],
        props.oil_properties);
    ++indices.pump_station_index;
    return model;
}

using hydro_factory_fn = std::unique_ptr<hydro_model_t> (*)(
    const chain_task_properties_t&,
    chain_build_indices_t&);

constexpr std::array<hydro_factory_fn, 4> k_hydro_factories = {
    &create_pipe_model,
    &create_local_resistance_model,
    &create_pump_model,
    &create_pump_station_model,
};

constexpr std::size_t type_index(const type_of_obj_t elem) {
    return static_cast<std::size_t>(elem);
}

bool pipe_result_ready(const pipe_task_result_t& result) {
    return !result.head_profile.empty() ||
        !result.pressure_profile.empty() ||
        std::isfinite(result.volume_flow);
}

bool local_resistance_result_ready(const local_resistanc_task_result_t& result) {
    return std::isfinite(result.pressure_in) ||
        std::isfinite(result.pressure_out) ||
        std::isfinite(result.volume_flow);
}

bool pump_result_ready(const pump_task_result_t& result) {
    return std::isfinite(result.pressure_in) ||
        std::isfinite(result.pressure_out) ||
        std::isfinite(result.volume_flow);
}

bool pump_station_result_ready(const pump_station_result_t& result) {
    return std::isfinite(result.pressure_in) ||
        std::isfinite(result.pressure_out) ||
        std::isfinite(result.volume_flow);
}

} // namespace

// ============================================================================
// hydro_model_t
// ============================================================================

double hydro_model_t::suggest_pp_initial_volume_flow() const {
    return std::numeric_limits<double>::quiet_NaN();
}

// ============================================================================
// chain_task_properties_t
// ============================================================================

chain_task_properties_t chain_task_properties_t::create_chain(std::vector<type_of_obj_t> chain) {
    chain_task_properties_t chain_task_properties;
    chain_task_properties.chain = std::move(chain);
    return chain_task_properties;
}

// ============================================================================
// hydro_chain_model_t
// ============================================================================

hydro_chain_model_t::hydro_chain_model_t(const chain_task_properties_t& chain_prop)
    : chain_task_properties{chain_prop}
{
}

void hydro_chain_model_t::ensure_chain_built() {
    if (owned_models_.empty() && !chain_task_properties.chain.empty()) {
        build_chain_models();
    }
}

void hydro_chain_model_t::ensure_not_null(const hydro_model_t* elem) {
    if (elem == nullptr) {
        throw std::runtime_error("В цепочке обнаружен nullptr");
    }
}

void hydro_chain_model_t::build_chain_models() {
    chain_has_type_ = {};
    chain_build_indices_t indices;
    owned_models_.reserve(chain_task_properties.chain.size());
    chain_models_.reserve(chain_task_properties.chain.size());

    for (const type_of_obj_t elem : chain_task_properties.chain) {
        const std::size_t idx = type_index(elem);
        if (idx >= k_hydro_factories.size()) {
            throw std::runtime_error("Неизвестный тип элемента цепочки");
        }
        chain_has_type_[idx] = true;
        owned_models_.push_back(k_hydro_factories[idx](chain_task_properties, indices));
        chain_models_.push_back(owned_models_.back().get());
    }
}

void hydro_chain_model_t::apply_pq_boundary(const double pressure_in_val, const double volume_flow_val) {
    pressure_in = pressure_in_val;
    volume_flow = volume_flow_val;
}

void hydro_chain_model_t::apply_qp_boundary(const double pressure_out_val, const double volume_flow_val) {
    pressure_out = pressure_out_val;
    volume_flow = volume_flow_val;
}

void hydro_chain_model_t::apply_pp_boundary(const double pressure_in_val, const double pressure_out_val) {
    pressure_in = pressure_in_val;
    pressure_out = pressure_out_val;
}

double hydro_chain_model_t::outlet_pressure_after_pq() const {
    return pressure_out;
}

double hydro_chain_model_t::inlet_pressure_after_qp() const {
    return pressure_in;
}

double hydro_chain_model_t::volume_flow_after_pp() const {
    return volume_flow;
}

void hydro_chain_model_t::commit_pq_result(chain_task_result_t& chain_result) const {
    chain_result = chain_task_result;
}

void hydro_chain_model_t::commit_qp_result(chain_task_result_t& chain_result) const {
    chain_result = chain_task_result;
}

void hydro_chain_model_t::commit_pp_result(chain_task_result_t& chain_result) const {
    chain_result = chain_task_result;
}

void hydro_chain_model_t::validate_pp_boundaries() const {
    if (std::isnan(pressure_in) || std::isnan(pressure_out)) {
        throw std::runtime_error("Не заданы граничные давления цепочки");
    }
}

double hydro_chain_model_t::outlet_pressure_for_pq_flow(const double q_trial) const {
    double p = pressure_in;
    for (hydro_model_t* elem : chain_models_) {
        ensure_not_null(elem);
        elem->apply_pq_boundary(p, q_trial);
        elem->solve_pq();
        p = elem->outlet_pressure_after_pq();
    }
    return p;
}

double hydro_chain_model_t::pp_residual(const double q_trial) const {
    return pressure_out - outlet_pressure_for_pq_flow(q_trial);
}

double hydro_chain_model_t::suggest_pp_initial_volume_flow_for_chain() const {
    for (hydro_model_t* elem : chain_models_) {
        ensure_not_null(elem);
        elem->apply_pp_boundary(pressure_in, pressure_out);
        const double hint = elem->suggest_pp_initial_volume_flow();
        if (std::isfinite(hint) && hint > 0.0) {
            return hint;
        }
    }
    return pp_solver_settings_.fallback_initial_volume_flow;
}

void hydro_chain_model_t::solve_pq() {
    ensure_chain_built();
    double p = pressure_in;
    chain_task_result.pressure_in = pressure_in;
    chain_task_result.volume_flow = volume_flow;

    for (hydro_model_t* elem : chain_models_) {
        ensure_not_null(elem);
        elem->apply_pq_boundary(p, volume_flow);
        elem->solve_pq();
        elem->commit_pq_result(chain_task_result);
        p = elem->outlet_pressure_after_pq();
    }

    pressure_out = p;
    chain_task_result.pressure_out = p;
}

void hydro_chain_model_t::solve_qp() {
    ensure_chain_built();
    double p = pressure_out;

    for (hydro_model_t* elem : std::views::reverse(chain_models_)) {
        ensure_not_null(elem);
        elem->apply_qp_boundary(p, volume_flow);
        elem->solve_qp();
        elem->commit_qp_result(chain_task_result);
        p = elem->inlet_pressure_after_qp();
    }

    pressure_in = p;
    chain_task_result.pressure_in = pressure_in;
    chain_task_result.volume_flow = volume_flow;
}

void hydro_chain_model_t::solve_pp() {
    ensure_chain_built();
    validate_pp_boundaries();

    if (std::abs(pp_residual(0.0)) < pp_solver_settings_.flow_zero_tolerance) {
        volume_flow = 0.0;
        solve_pq();
        chain_task_result.pressure_in = pressure_in;
        chain_task_result.pressure_out = pressure_out;
        chain_task_result.volume_flow = volume_flow;
        return;
    }

    fixed_scalar_wrapper_t equation(
        [this](const double q) { return pp_residual(q); },
        pp_solver_settings_.residual_tolerance
    );
    fixed_solver_parameters_t<1> parameters;
    parameters.iteration_count = pp_solver_settings_.max_iterations;
    parameters.argument_increment_norm = pp_solver_settings_.argument_tolerance;
    parameters.residuals_norm = pp_solver_settings_.residual_tolerance;
    parameters.residuals_norm_allow_early_exit = true;

    fixed_solver_result_t<1> result;
    fixed_newton_raphson<1>::solve(
        equation,
        suggest_pp_initial_volume_flow_for_chain(),
        parameters,
        &result,
        nullptr);
    if (result.result_code != numerical_result_code_t::Converged) {
        throw std::runtime_error("solve_pp: метод Ньютона не сошёлся");
    }

    volume_flow = result.argument;
    solve_pq();
    chain_task_result.pressure_in = pressure_in;
    chain_task_result.pressure_out = pressure_out;
    chain_task_result.volume_flow = volume_flow;
}

const chain_task_result_t& hydro_chain_model_t::get_chain_task_result() const {
    if (chain_has_type_[type_index(type_of_obj_t::pipe)] &&
        !pipe_result_ready(chain_task_result.pipe_task_result)) {
        throw std::runtime_error("Результат по трубе не готов");
    }
    if (chain_has_type_[type_index(type_of_obj_t::local_resistance)] &&
        !local_resistance_result_ready(chain_task_result.local_resistanc_task_result)) {
        throw std::runtime_error("Результат по местному сопротивлению не готов");
    }
    if (chain_has_type_[type_index(type_of_obj_t::pump)] &&
        !pump_result_ready(chain_task_result.pump_task_result)) {
        throw std::runtime_error("Результат по насосу не готов");
    }
    if (chain_has_type_[type_index(type_of_obj_t::pump_station)] &&
        !pump_station_result_ready(chain_task_result.pump_station_result)) {
        throw std::runtime_error("Результат по насосной станции не готов");
    }
    if (std::isnan(chain_task_result.pressure_in) &&
        std::isnan(chain_task_result.pressure_out) &&
        std::isnan(chain_task_result.volume_flow)) {
        throw std::runtime_error("Результат по цепочке не готов");
    }
    return chain_task_result;
}

} // namespace
