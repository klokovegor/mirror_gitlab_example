/// @brief Тесты входных структур и расчёта полиморфной гидравлической цепочки.

#include "hydraulic_chain.h"

#include "gtest/gtest.h"

#include <limits>

using namespace hydraulics_struct;

namespace {

oil_properties_t make_oil() {
    return oil_properties_t::create_oil(850.0, 1e-5);
}

pipe_properties_t make_pipe() {
    return pipe_properties_t::create_pipe(0.5, 0.01);
}

pump_properties_t make_pump() {
    return pump_properties_t::create_pump_properties(approximation_coefficients_t::HM_1250_260, 50.0, 50.0);
}

local_resistance_properties_t make_local_resistance() {
    return local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
}

void expect_runtime_error_contains(const std::function<void()>& fn, const std::string& expected_substring) {
    try {
        fn();
        FAIL() << "Ожидалось исключение std::runtime_error";
    } catch (const std::runtime_error& ex) {
        EXPECT_NE(std::string(ex.what()).find(expected_substring), std::string::npos);
    } catch (...) {
        FAIL() << "Ожидалось исключение std::runtime_error";
    }
}

chain_task_properties_t make_single_pipe_chain_props() {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pipe});
    props.oil_properties = make_oil();
    props.pipe_properties.push_back(make_pipe());
    props.pipe_profile.push_back(pipe_profile_t::create_profile(
        {0.0, 50.0, 100.0, 150.0}, {10.0, 12.0, 15.0, 18.0}));
    return props;
}

chain_task_properties_t make_mixed_chain_props() {
    chain_task_properties_t props = chain_task_properties_t::create_chain({
        type_of_obj_t::pipe,
        type_of_obj_t::pump,
        type_of_obj_t::local_resistance,
    });
    props.oil_properties = make_oil();
    props.pipe_properties.push_back(pipe_properties_t::create_pipe(0.2, 0.005));
    props.pipe_properties.push_back(pipe_properties_t::create_pipe(0.10, 0.005));
    props.pipe_profile.push_back(pipe_profile_t::create_profile(
        {0.0, 80.0, 160.0}, {5.0, 8.0, 11.0}));
    props.pump_properties.push_back(make_pump());
    props.local_resistance_properties.push_back(make_local_resistance());
    return props;
}

double manual_mixed_chain_pq_outlet(
    const chain_task_properties_t& props,
    const double pressure_in,
    const double volume_flow)
{
    const oil_properties_t& oil = props.oil_properties;

    pipe_calculator_t pipe(
        props.pipe_profile[0],
        props.pipe_properties[0],
        oil);
    pipe.apply_pq_boundary(pressure_in, volume_flow);
    pipe.solve_pq();
    double p = pipe.outlet_pressure_after_pq();

    pump_calculator_t pump(props.pump_properties[0], oil);
    pump.apply_pq_boundary(p, volume_flow);
    pump.solve_pq();
    p = pump.outlet_pressure_after_pq();

    local_resistance_calculator_t local(
        props.pipe_properties[1],
        oil,
        props.local_resistance_properties[0]);
    local.apply_pq_boundary(p, volume_flow);
    local.solve_pq();
    return local.outlet_pressure_after_pq();
}

double manual_mixed_chain_qp_inlet(
    const chain_task_properties_t& props,
    const double pressure_out,
    const double volume_flow)
{
    const oil_properties_t& oil = props.oil_properties;

    local_resistance_calculator_t local(
        props.pipe_properties[1],
        oil,
        props.local_resistance_properties[0]);
    local.apply_qp_boundary(pressure_out, volume_flow);
    local.solve_qp();
    double p = local.inlet_pressure_after_qp();

    pump_calculator_t pump(props.pump_properties[0], oil);
    pump.apply_qp_boundary(p, volume_flow);
    pump.solve_qp();
    p = pump.inlet_pressure_after_qp();

    pipe_calculator_t pipe(
        props.pipe_profile[0],
        props.pipe_properties[0],
        oil);
    pipe.apply_qp_boundary(p, volume_flow);
    pipe.solve_qp();
    return pipe.inlet_pressure_after_qp();
}

} // namespace

TEST(HydraulicChainProperties, CreateChainStoresOrder) {
    const std::vector<type_of_obj_t> chain = {
        type_of_obj_t::pipe,
        type_of_obj_t::pump,
        type_of_obj_t::local_resistance
    };
    const auto props = chain_task_properties_t::create_chain(chain);
    EXPECT_EQ(props.chain, chain);
    EXPECT_TRUE(props.pipe_profile.empty());
    EXPECT_TRUE(props.pipe_properties.empty());
    EXPECT_TRUE(props.local_resistance_properties.empty());
    EXPECT_TRUE(props.pump_properties.empty());
    EXPECT_TRUE(props.pump_station_properties.empty());
}

TEST(HydraulicChainCalculator, GetResultBeforeSolveThrows) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({});
    props.oil_properties = make_oil();
    const hydro_chain_model_t chain(props);
    EXPECT_THROW(static_cast<void>(chain.get_chain_task_result()), std::runtime_error);
}

TEST(HydraulicChainCalculator, SolvePqRequiresPipeProperties) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pipe});
    props.oil_properties = make_oil();
    hydro_chain_model_t chain(props);
    expect_runtime_error_contains([&chain]() { chain.solve_pq(); }, "pipe_properties");
}

TEST(HydraulicChainCalculator, SolvePqRequiresPipeProfile) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pipe});
    props.oil_properties = make_oil();
    props.pipe_properties.push_back(make_pipe());
    hydro_chain_model_t chain(props);
    expect_runtime_error_contains([&chain]() { chain.solve_pq(); }, "pipe_profile");
}

TEST(HydraulicChainCalculator, SolvePqRequiresLocalResistanceProperties) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::local_resistance});
    props.oil_properties = make_oil();
    props.pipe_properties.push_back(make_pipe());
    hydro_chain_model_t chain(props);
    expect_runtime_error_contains([&chain]() { chain.solve_pq(); }, "local_resistance_properties");
}

TEST(HydraulicChainCalculator, SolvePqRequiresPumpProperties) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pump});
    props.oil_properties = make_oil();
    hydro_chain_model_t chain(props);
    expect_runtime_error_contains([&chain]() { chain.solve_pq(); }, "pump_properties");
}

TEST(HydraulicChainCalculator, SolvePqRequiresPumpStationProperties) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pump_station});
    props.oil_properties = make_oil();
    hydro_chain_model_t chain(props);
    expect_runtime_error_contains([&chain]() { chain.solve_pq(); }, "pump_station_properties");
}

TEST(HydraulicChainCalculator, SolvePqMixedChainPropagatesPressure) {
    const chain_task_properties_t props = make_mixed_chain_props();
    hydro_chain_model_t chain(props);
    chain.pressure_in = 500000.0;
    chain.volume_flow = 0.03;
    const double expected_out = manual_mixed_chain_pq_outlet(props, chain.pressure_in, chain.volume_flow);
    // Act
    chain.solve_pq();
    // Assert
    EXPECT_NEAR(chain.pressure_out, expected_out, 1.0);
    EXPECT_NEAR(chain.get_chain_task_result().pressure_out, expected_out, 1.0);
}

TEST(HydraulicChainCalculator, GetChainTaskResultAfterSolvePqPipeOnly) {
    hydro_chain_model_t chain(make_single_pipe_chain_props());
    chain.pressure_in = 500000.0;
    chain.volume_flow = 0.04;
    // Act
    chain.solve_pq();
    const chain_task_result_t& result = chain.get_chain_task_result();
    // Assert
    EXPECT_NEAR(result.volume_flow, chain.volume_flow, 1e-12);
    EXPECT_NEAR(result.pressure_in, chain.pressure_in, 1.0);
    EXPECT_NEAR(result.pressure_out, chain.pressure_out, 1.0);
    EXPECT_FALSE(result.pipe_task_result.pressure_profile.empty());
}

TEST(HydraulicChainCalculator, SolvePpRequiresBoundaries) {
    hydro_chain_model_t chain(make_single_pipe_chain_props());
    expect_runtime_error_contains([&chain]() { chain.solve_pp(); }, "граничные давления");
}

TEST(HydraulicChainCalculator, SolvePpSinglePipeRoundTrip) {
    hydro_chain_model_t chain(make_single_pipe_chain_props());
    chain.pressure_in = 500000.0;
    chain.volume_flow = 0.04;
    chain.solve_pq();
    const double q_ref = chain.volume_flow;
    const double p_out = chain.get_chain_task_result().pipe_task_result.pressure_profile.back();
    chain.volume_flow = std::numeric_limits<double>::quiet_NaN();
    chain.pressure_out = p_out;

    ASSERT_NO_THROW(chain.solve_pp());
    EXPECT_NEAR(chain.volume_flow, q_ref, 1e-4);
    EXPECT_NEAR(chain.get_chain_task_result().volume_flow, q_ref, 1e-4);

    chain.solve_pq();
    EXPECT_NEAR(chain.pressure_out, p_out, 1.0);
    EXPECT_NEAR(
        chain.get_chain_task_result().pipe_task_result.pressure_profile.back(),
        p_out,
        1.0);
}

TEST(HydraulicChainCalculator, SolvePpMixedChainRoundTrip) {
    hydro_chain_model_t chain(make_mixed_chain_props());
    chain.pressure_in = 500000.0;
    chain.volume_flow = 0.03;
    chain.solve_pq();
    const double q_ref = chain.volume_flow;
    const double p_out = chain.pressure_out;
    chain.volume_flow = std::numeric_limits<double>::quiet_NaN();
    chain.pressure_out = p_out;

    ASSERT_NO_THROW(chain.solve_pp());
    EXPECT_NEAR(chain.volume_flow, q_ref, 1e-4);

    chain.solve_pq();
    EXPECT_NEAR(chain.pressure_out, p_out, 1.0);
}
