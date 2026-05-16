/// @brief Тесты коэффициентов ξ, фабрик свойств и калькулятора местного сопротивления.

#include "hydraulic_chain.h"

#include "gtest/gtest.h"

using namespace hydraulics_struct;

/// @brief Проверяет, что фабрика местного сопротивления корректно копирует переданные поля.
TEST(LocalResistanceProperties, FactoryCopiesFields) {
    // Arrange
    const double user_xi = 0.33;
    const double d_ref = 0.11;
    const double d_pair = 0.07;
    // Act
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        user_xi, local_resistance_kind_t::rotation_pipe_60, d_ref, d_pair);
    // Assert
    EXPECT_EQ(local_resistance.kind, local_resistance_kind_t::rotation_pipe_60);
    EXPECT_DOUBLE_EQ(local_resistance.reference_inner_diameter, d_ref);
    EXPECT_DOUBLE_EQ(local_resistance.paired_inner_diameter, d_pair);
    EXPECT_DOUBLE_EQ(local_resistance.user_local_resistance_coefficient, user_xi);
}

/// @brief Проверяет формулы коэффициента для внезапного расширения и сужения.
TEST(LocalResistanceFunctions, SuddenExpansionAndNarrowingMatchFormula) {
    // Arrange
    const double d_small = 0.05;
    const double d_large = 0.10;
    const double ratio = d_small / d_large;
    const double expected = std::pow(1.0 - ratio * ratio, 2.0);
    // Act
    const double xi_exp = local_resistance_coefficient_sudden_expansion(d_small, d_large);
    const double xi_narrow = local_resistance_coefficient_sudden_narrowing(d_large, d_small);
    // Assert
    EXPECT_NEAR(xi_exp, expected, 1e-12);
    EXPECT_NEAR(xi_narrow, expected, 1e-12);
}

/// @brief Проверяет формулу коэффициента местного сопротивления для диафрагмы.
TEST(LocalResistanceFunctions, DiaphragmMatchesFormula) {
    // Arrange
    const double d_pipe = 0.12;
    const double d_diaphragm = 0.06;
    const double ratio = d_diaphragm / d_pipe;
    const double epsilon = 0.57 + 0.043 / (1.1 - ratio);
    const double expected = std::pow(1.0 / (ratio * epsilon) - 1.0, 2.0);
    // Act
    const double xi = local_resistance_coefficient_diaphragm(d_pipe, d_diaphragm);
    // Assert
    EXPECT_NEAR(xi, expected, 1e-12);
}

/// @brief Проверяет, что невалидная геометрия даёт NaN в свободных функциях.
TEST(LocalResistanceFunctions, InvalidGeometryReturnsNaN) {
    // Arrange & Act & Assert
    EXPECT_TRUE(std::isnan(local_resistance_coefficient_sudden_expansion(0.0, 0.1)));
    EXPECT_TRUE(std::isnan(local_resistance_coefficient_sudden_expansion(0.2, 0.1)));

    EXPECT_TRUE(std::isnan(local_resistance_coefficient_sudden_narrowing(0.0, 0.1)));
    EXPECT_TRUE(std::isnan(local_resistance_coefficient_sudden_narrowing(0.1, 0.1)));

    EXPECT_TRUE(std::isnan(local_resistance_coefficient_diaphragm(0.0, 0.1)));
    EXPECT_TRUE(std::isnan(local_resistance_coefficient_diaphragm(0.1, 0.1)));
    EXPECT_TRUE(std::isnan(local_resistance_coefficient_diaphragm(0.1, 0.2)));
}

/// @brief Проверяет коэффициенты для константных и пользовательского типов местного сопротивления.
TEST(LocalResistanceProperties, CoefficientByKind) {
    // Arrange
    local_resistance_properties_t gate{};
    gate.kind = local_resistance_kind_t::gate_valve_open;
    local_resistance_properties_t user{};
    user.kind = local_resistance_kind_t::user_local_resistance_coeff;
    user.user_local_resistance_coefficient = 0.42;
    local_resistance_properties_t user_nan{};
    user_nan.kind = local_resistance_kind_t::user_local_resistance_coeff;
    // Act
    const double xi_gate = gate.calc_local_resistance_coefficient();
    const double xi_user = user.calc_local_resistance_coefficient();
    const double xi_user_nan = user_nan.calc_local_resistance_coefficient();
    // Assert
    EXPECT_DOUBLE_EQ(xi_gate, 0.15);
    EXPECT_DOUBLE_EQ(xi_user, 0.42);
    EXPECT_TRUE(std::isnan(xi_user_nan));
}

/// @brief Проверяет коэффициенты для всех фиксированных поворотных элементов.
TEST(LocalResistanceProperties, RotationKindsReturnExpectedConstants) {
    // Arrange
    local_resistance_properties_t rot90{};
    rot90.kind = local_resistance_kind_t::rotation_pipe_90;
    local_resistance_properties_t rot45{};
    rot45.kind = local_resistance_kind_t::rotation_pipe_45;
    local_resistance_properties_t rot30{};
    rot30.kind = local_resistance_kind_t::rotation_pipe_30;
    // Act
    const double xi90 = rot90.calc_local_resistance_coefficient();
    const double xi45 = rot45.calc_local_resistance_coefficient();
    const double xi30 = rot30.calc_local_resistance_coefficient();
    // Assert
    EXPECT_DOUBLE_EQ(xi90, 0.5);
    EXPECT_DOUBLE_EQ(xi45, 0.3);
    EXPECT_DOUBLE_EQ(xi30, 0.2);
}

/// @brief Проверяет вызов свободной функции через поведение класса для внезапного расширения.
TEST(LocalResistanceProperties, SuddenExpansionKindDelegatesToFreeFunction) {
    // Arrange
    local_resistance_properties_t local_resistance{};
    local_resistance.kind = local_resistance_kind_t::sudden_expansion;
    local_resistance.reference_inner_diameter = 0.05;
    local_resistance.paired_inner_diameter = 0.10;
    const double expected = local_resistance_coefficient_sudden_expansion(0.05, 0.10);
    // Act
    const double xi = local_resistance.calc_local_resistance_coefficient();
    // Assert
    EXPECT_NEAR(xi, expected, 1e-12);
}

/// @brief Проверяет вызов свободной функции через поведение класса для внезапного сужения.
TEST(LocalResistanceProperties, SuddenNarrowingKindDelegatesToFreeFunction) {
    // Arrange
    local_resistance_properties_t local_resistance{};
    local_resistance.kind = local_resistance_kind_t::sudden_narrowing;
    local_resistance.reference_inner_diameter = 0.10;
    local_resistance.paired_inner_diameter = 0.05;
    const double expected = local_resistance_coefficient_sudden_narrowing(0.10, 0.05);
    // Act
    const double xi = local_resistance.calc_local_resistance_coefficient();
    // Assert
    EXPECT_NEAR(xi, expected, 1e-12);
}

/// @brief Проверяет вызов свободной функции через поведение класса для диафрагмы.
TEST(LocalResistanceProperties, DiaphragmKindDelegatesToFreeFunction) {
    // Arrange
    local_resistance_properties_t local_resistance{};
    local_resistance.kind = local_resistance_kind_t::diaphragm;
    local_resistance.reference_inner_diameter = 0.12;
    local_resistance.paired_inner_diameter = 0.06;
    const double expected = local_resistance_coefficient_diaphragm(0.12, 0.06);
    // Act
    const double xi = local_resistance.calc_local_resistance_coefficient();
    // Assert
    EXPECT_NEAR(xi, expected, 1e-12);
}

/// @brief Проверяет, что результат недоступен до выполнения расчёта местного сопротивления.
TEST(LocalResistanceCalculator, ResultUnavailableBeforeSolve) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(860.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    const local_resistance_calculator_t calc(pipe, oil, local_resistance);
    // Act & Assert
    EXPECT_THROW(static_cast<void>(calc.get_local_resistanc_task_result()), std::runtime_error);
}

/// @brief Проверяет формулу PQ-расчёта местного сопротивления.
TEST(LocalResistanceCalculator, SolvePqMatchesPressureDropFormula) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(860.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_start = 320000.0;
    calc.volume_flow = 0.02;
    const double xi = local_resistance.calc_local_resistance_coefficient();
    const double expected = calc.pressure_start - xi * oil.density / (2.0 * std::pow(pipe.get_pipe_area(), 2.0)) *
        calc.volume_flow * std::abs(calc.volume_flow);
    // Act
    calc.solve_pq();
    // Assert
    EXPECT_NEAR(calc.get_local_resistanc_task_result().pressure_out, expected, 1e-6);
}

/// @brief Проверяет формулу QP-расчёта местного сопротивления.
TEST(LocalResistanceCalculator, SolveQpMatchesPressureGainFormula) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(900.0, 1.1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_end = 180000.0;
    calc.volume_flow = 0.015;
    const double xi = local_resistance.calc_local_resistance_coefficient();
    const double expected = calc.pressure_end + xi * oil.density / (2.0 * std::pow(pipe.get_pipe_area(), 2.0)) *
        calc.volume_flow * std::abs(calc.volume_flow);
    // Act
    calc.solve_qp();
    // Assert
    EXPECT_NEAR(calc.get_local_resistanc_task_result().pressure_in, expected, 1e-6);
}

/// @brief Проверяет, что при нулевом расходе давление не меняется в задаче PQ.
TEST(LocalResistanceCalculator, SolvePqWithZeroFlowKeepsPressure) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(860.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_start = 250000.0;
    calc.volume_flow = 0.0;
    // Act
    calc.solve_pq();
    // Assert
    EXPECT_DOUBLE_EQ(calc.get_local_resistanc_task_result().pressure_out, calc.pressure_start);
}

/// @brief Проверяет, что при нулевом расходе давление не меняется в задаче QP.
TEST(LocalResistanceCalculator, SolveQpWithZeroFlowKeepsPressure) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(860.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_end = 250000.0;
    calc.volume_flow = 0.0;
    // Act
    calc.solve_qp();
    // Assert
    EXPECT_DOUBLE_EQ(calc.get_local_resistanc_task_result().pressure_in, calc.pressure_end);
}

/// @brief Проверяет ошибку при отрицательном выходном давлении в задаче PQ.
TEST(LocalResistanceCalculator, SolvePqThrowsOnNegativePressureOut) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(860.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_start = 1.0;
    calc.volume_flow = 0.6;
    // Act & Assert
    EXPECT_THROW(calc.solve_pq(), std::runtime_error);
}

/// @brief Проверяет ошибку при отрицательном входном давлении в задаче QP.
TEST(LocalResistanceCalculator, SolveQpThrowsOnNegativePressureIn) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(860.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_end = 1.0;
    calc.volume_flow = -0.6;
    // Act & Assert
    EXPECT_THROW(calc.solve_qp(), std::runtime_error);
}

/// @brief Проверяет PP-расчёт местного сопротивления по формуле лекции.
TEST(LocalResistanceCalculator, SolvePpMatchesFormula) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.10, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5);
    const auto local_resistance = local_resistance_properties_t::create_local_resistance(
        0.15, local_resistance_kind_t::gate_valve_open, 0.1, 0.1);
    local_resistance_calculator_t calc(pipe, oil, local_resistance);
    calc.pressure_start = 300000.0;
    calc.pressure_end = 150000.0;
    const double xi = local_resistance.calc_local_resistance_coefficient();
    const int sign = (calc.pressure_start - calc.pressure_end >= 0) ? 1 : -1;
    const double expected = sign * std::sqrt(
        2.0 * std::pow(pipe.get_pipe_area(), 2) /
        (oil.density * xi) * std::abs(calc.pressure_start - calc.pressure_end));
    // Act & Assert
    ASSERT_NO_THROW(calc.solve_pp());
    EXPECT_NEAR(calc.get_local_resistanc_task_result().volume_flow, expected, 1e-6);
}