/// @brief Тесты входных структур и валидации калькулятора гидравлической цепочки.

#include "hydraulic_chain.h"

#include "gtest/gtest.h"

using namespace hydraulics_struct;

namespace {

oil_properties_t make_oil() {
    return oil_properties_t::create_oil(850.0, 1e-5);
}

pipe_properties_t make_pipe() {
    return pipe_properties_t::create_pipe(0.5, 0.01);
}

void expect_runtime_error_contains(const std::function<void()>& fn, const std::string& expected_substring) {
    // Act
    try {
        fn();
        // Assert
        FAIL() << "Ожидалось исключение std::runtime_error";
    } catch (const std::runtime_error& ex) {
        EXPECT_NE(std::string(ex.what()).find(expected_substring), std::string::npos);
    } catch (...) {
        FAIL() << "Ожидалось исключение std::runtime_error";
    }
}

} // namespace

TEST(HydraulicChainProperties, CreateChainStoresOrder) {
    // Arrange
    const std::vector<type_of_obj_t> chain = {
        type_of_obj_t::pipe,
        type_of_obj_t::pump,
        type_of_obj_t::local_resistance
    };
    // Act
    const auto props = chain_task_properties_t::create_chain(chain);
    // Assert
    EXPECT_EQ(props.chain, chain);
    EXPECT_TRUE(props.pipe_profile.empty());
    EXPECT_TRUE(props.pipe_properties.empty());
    EXPECT_TRUE(props.local_resistance_properties.empty());
    EXPECT_TRUE(props.pump_properties.empty());
    EXPECT_TRUE(props.pump_station_properties.empty());
}

TEST(HydraulicChainCalculator, GetResultBeforeSolveThrows) {
    // Arrange
    chain_task_properties_t props = chain_task_properties_t::create_chain({});
    props.oil_properties = make_oil();
    const chain_task_calculator_t calc(props);
    // Act & Assert
    EXPECT_THROW(static_cast<void>(calc.get_chain_task_result()), std::runtime_error);
}

TEST(HydraulicChainCalculator, SolvePqRequiresPipeProperties) {
    // Arrange
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pipe});
    props.oil_properties = make_oil();
    chain_task_calculator_t calc(props);
    // Act & Assert
    expect_runtime_error_contains([&calc]() { calc.solve_pq(); }, "pipe_properties");
}

TEST(HydraulicChainCalculator, SolvePqRequiresPipeProfile) {
    // Arrange
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pipe});
    props.oil_properties = make_oil();
    props.pipe_properties.push_back(make_pipe());
    chain_task_calculator_t calc(props);
    // Act & Assert
    expect_runtime_error_contains([&calc]() { calc.solve_pq(); }, "pipe_profile");
}

TEST(HydraulicChainCalculator, SolvePqRequiresLocalResistanceProperties) {
    // Arrange
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::local_resistance});
    props.oil_properties = make_oil();
    props.pipe_properties.push_back(make_pipe());
    chain_task_calculator_t calc(props);
    // Act & Assert
    expect_runtime_error_contains([&calc]() { calc.solve_pq(); }, "local_resistance_properties");
}

TEST(HydraulicChainCalculator, SolvePqRequiresPumpProperties) {
    // Arrange
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pump});
    props.oil_properties = make_oil();
    chain_task_calculator_t calc(props);
    // Act & Assert
    expect_runtime_error_contains([&calc]() { calc.solve_pq(); }, "pump_properties");
}

TEST(HydraulicChainCalculator, SolvePqRequiresPumpStationProperties) {
    // Arrange
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pump_station});
    props.oil_properties = make_oil();
    chain_task_calculator_t calc(props);
    // Act & Assert
    expect_runtime_error_contains([&calc]() { calc.solve_pq(); }, "pump_station_properties");
}
