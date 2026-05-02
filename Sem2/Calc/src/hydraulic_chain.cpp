/// @brief Реализация валидации входов и прохода по цепочке для solve_pq / solve_qp / solve_pp.

#include "hydraulic_chain.h"

namespace hydraulics_struct {

// ============================================================================
// chain_task_properties_t
// ============================================================================

/// @brief Заполняет только последовательность типов элементов; остальные векторы пустые.
chain_task_properties_t chain_task_properties_t::create_chain(std::vector<type_of_obj_t> chain) {
    chain_task_properties_t chain_task_properties;
    chain_task_properties.chain = chain;
    return chain_task_properties;
}

// ============================================================================
// chain_task_calculator_t
// ============================================================================

/// @brief Проверяет наличие профиля и свойств трубы.
void chain_task_calculator_t::validate_pipe_inputs() const {
    if (chain_task_properties.pipe_properties.empty()) {
        throw std::runtime_error("Для расчета трубы требуется pipe_properties");
    }
    if (chain_task_properties.pipe_profile.empty()) {
        throw std::runtime_error("Для расчета трубы требуется pipe_profile");
    }
    /* if (chain_task_properties.oil_properties.empty()) {
        throw std::runtime_error("Для расчета трубы требуется oil_properties");
    } */
}

/// @brief Проверяет наличие свойств местного сопротивления и трубы.
void chain_task_calculator_t::validate_local_resistance_inputs() const {
    if (chain_task_properties.local_resistance_properties.empty()) {
        throw std::runtime_error("Для расчета местного сопротивления требуется local_resistance_properties");
    }
    if (chain_task_properties.pipe_properties.empty()) {
        throw std::runtime_error("Для расчета местного сопротивления требуется pipe_properties");
    }
    /* if (chain_task_properties.oil_properties.empty()) {
        throw std::runtime_error("Для расчета местного сопротивления требуется oil_properties");
    } */
}

/// @brief Проверяет наличие параметров насоса.
void chain_task_calculator_t::validate_pump_inputs() const {
    if (chain_task_properties.pump_properties.empty()) {
        throw std::runtime_error("Для расчета насоса требуется pump_properties");
    }
    /* if (chain_task_properties.oil_properties.empty()) {
        throw std::runtime_error("Для расчета насоса требуется oil_properties");
    } */
}

/// @brief Проверяет наличие параметров насосной станции.
void chain_task_calculator_t::validate_pump_station_inputs() const {
    if (chain_task_properties.pump_station_properties.empty()) {
        throw std::runtime_error("Для расчета насосной станции требуется pump_station_properties");
    }
    /* if (chain_task_properties.oil_properties.empty()) {
        throw std::runtime_error("Для расчета насосной станции требуется oil_properties");
    } */
}

/// @brief Проход по цепочке в прямом порядке: известны P_in и Q.
void chain_task_calculator_t::solve_pq() {
    size_t pipe_index = 0;
    size_t local_resist_index = 0;
    size_t pump_index = 0;
    size_t pump_station_index = 0;
    for (auto& elem: chain_task_properties.chain) {
        switch (elem) {
        case type_of_obj_t::pipe: {
            validate_pipe_inputs();
            pipe_calculator_t calc_pipe(chain_task_properties.pipe_profile[pipe_index], chain_task_properties.pipe_properties[pipe_index], chain_task_properties.oil_properties);
            calc_pipe.pressure_start = pressure_in;
            calc_pipe.volume_flow = volume_flow;
            calc_pipe.solve_pq();
            pressure_out = chain_task_result.pipe_task_result.pressure_profile.back();
            ++pipe_index; }
        case type_of_obj_t::local_resistance: {
            validate_local_resistance_inputs();
            local_resistance_calculator_t calc_local(chain_task_properties.pipe_properties[pipe_index], chain_task_properties.oil_properties, chain_task_properties.local_resistance_properties[local_resist_index]);
            calc_local.pressure_start = pressure_in;
            calc_local.volume_flow = volume_flow;
            calc_local.solve_pq();
            pressure_out = chain_task_result.local_resistanc_task_result.pressure_out;
            ++local_resist_index; }
        case type_of_obj_t::pump: {
            validate_pump_inputs();
            pump_calculator_t calc_pump(chain_task_properties.pump_properties[pump_index], chain_task_properties.oil_properties);
            calc_pump.pressure_start = pressure_in;
            calc_pump.volume_flow = volume_flow;
            calc_pump.solve_pq();
            pressure_out = chain_task_result.pump_task_result.pressure_out;
            ++pump_index; }
        case type_of_obj_t::pump_station: {
            validate_pump_station_inputs();
            pump_station_calculator_t calc_pump_station(chain_task_properties.pump_station_properties[pump_station_index], chain_task_properties.oil_properties);
            calc_pump_station.pressure_start = pressure_in;
            calc_pump_station.volume_flow = volume_flow;
            calc_pump_station.solve_pq();
            pressure_out = chain_task_result.pump_station_result.pressure_out;
            ++pump_station_index; }
        }
    }
    chain_task_result.pressure_out = pressure_out;
}

/// @brief Проход по цепочке в обратном порядке: известны P_out и Q.
void chain_task_calculator_t::solve_qp() {
    size_t pipe_index = 0;
    size_t local_resist_index = 0;
    size_t pump_index = 0;
    size_t pump_station_index = 0;
    for (auto& elem: std::views::reverse(chain_task_properties.chain)) {
        switch (elem) {
        case type_of_obj_t::pipe: {
            validate_pipe_inputs();
            pipe_calculator_t calc_pipe(chain_task_properties.pipe_profile[pipe_index], chain_task_properties.pipe_properties[pipe_index], chain_task_properties.oil_properties);
            calc_pipe.pressure_end = pressure_out;
            calc_pipe.volume_flow = volume_flow;
            calc_pipe.solve_qp();
            pressure_in = chain_task_result.pipe_task_result.pressure_profile[0];
            ++pipe_index; }
        case type_of_obj_t::local_resistance: {
            validate_local_resistance_inputs();
            local_resistance_calculator_t calc_local(chain_task_properties.pipe_properties[pipe_index], chain_task_properties.oil_properties, chain_task_properties.local_resistance_properties[local_resist_index]);
            calc_local.pressure_end = pressure_out;
            calc_local.volume_flow = volume_flow;
            calc_local.solve_qp();
            pressure_in = chain_task_result.local_resistanc_task_result.pressure_in;
            ++local_resist_index; }
        case type_of_obj_t::pump: {
            validate_pump_inputs();
            pump_calculator_t calc_pump(chain_task_properties.pump_properties[pump_index], chain_task_properties.oil_properties);
            calc_pump.pressure_end = pressure_out;
            calc_pump.volume_flow = volume_flow;
            calc_pump.solve_qp();
            pressure_in = chain_task_result.pump_task_result.pressure_in;
            ++pump_index; }
        case type_of_obj_t::pump_station: {
            validate_pump_station_inputs();
            pump_station_calculator_t calc_pump_station(chain_task_properties.pump_station_properties[pump_station_index], chain_task_properties.oil_properties);
            calc_pump_station.pressure_end = pressure_out;
            calc_pump_station.volume_flow = volume_flow;
            calc_pump_station.solve_qp();
            pressure_in = chain_task_result.pump_station_result.pressure_in;
            ++pump_station_index; }
        }
    }
    chain_task_result.pressure_in = pressure_in;
}

/// @brief Проход по цепочке: известны давления на концах, ищется расход.
void chain_task_calculator_t::solve_pp() {
    size_t pipe_index = 0;
    size_t local_resist_index = 0;
    size_t pump_index = 0;
    size_t pump_station_index = 0;
    for (auto& elem: chain_task_properties.chain) {
        switch (elem) {
        case type_of_obj_t::pipe: {
            validate_pipe_inputs();
            pipe_calculator_t calc_pipe(chain_task_properties.pipe_profile[pipe_index], chain_task_properties.pipe_properties[pipe_index], chain_task_properties.oil_properties);
            calc_pipe.pressure_start = pressure_in;
            calc_pipe.pressure_end = pressure_out;
            calc_pipe.solve_pp();
            volume_flow = chain_task_result.pipe_task_result.volume_flow;
            ++pipe_index; }
        case type_of_obj_t::local_resistance: {
            validate_local_resistance_inputs();
            local_resistance_calculator_t calc_local(chain_task_properties.pipe_properties[pipe_index], chain_task_properties.oil_properties, chain_task_properties.local_resistance_properties[local_resist_index]);
            calc_local.pressure_start = pressure_in;
            calc_local.pressure_end = pressure_out;
            calc_local.solve_pp();
            volume_flow = chain_task_result.local_resistanc_task_result.volume_flow;
            ++local_resist_index; }
        case type_of_obj_t::pump: {
            validate_pump_inputs();
            pump_calculator_t calc_pump(chain_task_properties.pump_properties[pump_index], chain_task_properties.oil_properties);
            calc_pump.pressure_start = pressure_in;
            calc_pump.pressure_end = pressure_out;
            calc_pump.solve_pp();
            volume_flow = chain_task_result.pump_task_result.volume_flow;
            ++pump_index; }
        case type_of_obj_t::pump_station: {
            validate_pump_station_inputs();
            pump_station_calculator_t calc_pump_station(chain_task_properties.pump_station_properties[pump_station_index], chain_task_properties.oil_properties);
            calc_pump_station.pressure_start = pressure_in;
            calc_pump_station.pressure_end = pressure_out;
            calc_pump_station.solve_pp();
            volume_flow = chain_task_result.pump_station_result.volume_flow;
            ++pump_station_index; }
        }
    }
    chain_task_result.volume_flow = volume_flow;
}

/// @brief Возвращает результат, если по всем веткам заполнены величины; иначе исключение.
const chain_task_result_t& chain_task_calculator_t::get_chain_task_result() const {
    if (chain_task_result.pipe_task_result.head_profile.empty() && chain_task_result.pipe_task_result.pressure_profile.empty() && std::isnan(chain_task_result.pipe_task_result.volume_flow)) {
        throw std::runtime_error("Результат по трубе не готов");
    }
    if (std::isnan(chain_task_result.local_resistanc_task_result.pressure_in) && std::isnan(chain_task_result.local_resistanc_task_result.pressure_out) && 
                            std::isnan(chain_task_result.local_resistanc_task_result.volume_flow)) {
        throw std::runtime_error("Результат по местному сопротивлению не готов");
    }
    if (std::isnan(chain_task_result.pump_task_result.pressure_in) && std::isnan(chain_task_result.pump_task_result.pressure_out) && 
                            std::isnan(chain_task_result.pump_task_result.volume_flow)) {
        throw std::runtime_error("Результат по насосу не готов");
    }
    if (std::isnan(chain_task_result.pump_station_result.pressure_in) && std::isnan(chain_task_result.pump_station_result.pressure_out) && 
                            std::isnan(chain_task_result.pump_station_result.volume_flow)) {
        throw std::runtime_error("Результат по насосной станции не готов");
    }
    if (std::isnan(chain_task_result.pressure_in) && std::isnan(chain_task_result.pressure_out) && 
                            std::isnan(chain_task_result.volume_flow)) {
        throw std::runtime_error("Результат по цепочке не готов");
    }
    return chain_task_result;
}

} // namespace