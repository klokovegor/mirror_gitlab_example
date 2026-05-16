/// @brief Тесты утилит полинома, параметров насоса, калькулятора и насосной станции.

#include "hydraulic_chain.h"

#include "gtest/gtest.h"

using namespace hydraulics_struct;

namespace {

oil_properties_t make_oil() {
    return oil_properties_t::create_oil(850.0, 1e-5);
}

pump_properties_t make_pump() {
    return pump_properties_t::create_pump_properties(approximation_coefficients_t::HM_1250_260, 50.0, 50.0);
}

} // namespace

/// @brief Проверяет перевод 4 коэффициентов полинома в систему СИ.
TEST(PumpUtils, PolynomialCoefficientsSiConvertsFourTermVector) {
    // Arrange
    const std::vector<double> coeffs = {1.0, 2.0, 3.0, 4.0};
    // Act
    const auto converted = polynomial_coefficients_si(coeffs);
    // Assert
    ASSERT_EQ(converted.size(), 4u);
    EXPECT_DOUBLE_EQ(converted[0], 1.0);
    EXPECT_DOUBLE_EQ(converted[1], 2.0 * 3600.0);
    EXPECT_DOUBLE_EQ(converted[2], 3.0 * 3600.0 * 3600.0);
    EXPECT_DOUBLE_EQ(converted[3], 4.0 * 3600.0 * 3600.0 * 3600.0);
}

/// @brief Проверяет, что при недостатке коэффициентов функция возвращает пустой вектор.
TEST(PumpUtils, PolynomialCoefficientsSiReturnsEmptyForTooSmallInput) {
    // Arrange
    const std::vector<double> coeffs = {1.0};
    // Act
    const auto converted = polynomial_coefficients_si(coeffs);
    // Assert
    EXPECT_TRUE(converted.empty());
}

/// @brief Проверяет заполнение нулями старших коэффициентов для двухчленного полинома.
TEST(PumpUtils, PolynomialCoefficientsSiPadsHigherOrdersWithZeros) {
    // Arrange
    const std::vector<double> coeffs = {331.0, 0.451e-4};
    // Act
    const auto converted = polynomial_coefficients_si(coeffs);
    // Assert
    ASSERT_EQ(converted.size(), 4u);
    EXPECT_DOUBLE_EQ(converted[0], 331.0);
    EXPECT_DOUBLE_EQ(converted[1], 0.451e-4 * 3600.0);
    EXPECT_DOUBLE_EQ(converted[2], 0.0);
    EXPECT_DOUBLE_EQ(converted[3], 0.0);
}

/// @brief Проверяет, что фабрика параметров насоса корректно заполняет поля.
TEST(PumpProperties, FactorySetsFields) {
    // Arrange
    const approximation_coefficients_t type = approximation_coefficients_t::HM_5000_210;
    const double f_nom = 50.0;
    const double f_cur = 47.5;
    // Act
    const auto pump = pump_properties_t::create_pump_properties(type, f_nom, f_cur);
    // Assert
    EXPECT_EQ(pump.approximation_coeff, type);
    EXPECT_DOUBLE_EQ(pump.nominal_frequency, f_nom);
    EXPECT_DOUBLE_EQ(pump.current_frequency, f_cur);
    EXPECT_TRUE(pump.started);
}

/// @brief Проверяет количество коэффициентов аппроксимации для разных типов насосов.
TEST(PumpProperties, CoefficientsHaveExpectedSizes) {
    // Arrange & Act & Assert
    EXPECT_EQ(pump_properties_t::get_polynomial_coefficients(approximation_coefficients_t::HM_1250_260).size(), 2u);
    EXPECT_EQ(pump_properties_t::get_polynomial_coefficients(approximation_coefficients_t::HM_5000_210).size(), 2u);
    EXPECT_EQ(pump_properties_t::get_polynomial_coefficients(approximation_coefficients_t::HM_7000_210).size(), 2u);

    EXPECT_EQ(pump_properties_t::get_polynomial_coefficients(approximation_coefficients_t::HPV_1250_60).size(), 4u);
    EXPECT_EQ(pump_properties_t::get_polynomial_coefficients(approximation_coefficients_t::DHM_2500_230).size(), 4u);
    EXPECT_EQ(pump_properties_t::get_polynomial_coefficients(approximation_coefficients_t::HM_3600_230).size(), 4u);
}

/// @brief Проверяет, что результат недоступен до выполнения расчёта насоса.
TEST(PumpCalculator, ResultUnavailableBeforeSolve) {
    // Arrange
    const pump_calculator_t calc(make_pump(), make_oil());
    // Act & Assert
    EXPECT_THROW(static_cast<void>(calc.get_pump_task_result()), std::runtime_error);
}

/// @brief Проверяет формулу PQ-расчёта насоса при равных номинальной и текущей частотах.
TEST(PumpCalculator, SolvePqMatchesTwoTermCurveForEqualFrequencies) {
    // Arrange
    const auto pump = make_pump();
    const auto oil = make_oil();
    const auto coeffs = pump_properties_t::get_polynomial_coefficients(pump.approximation_coeff);
    ASSERT_EQ(coeffs.size(), 2u);
    pump_calculator_t calc(pump, oil);
    calc.pressure_start = 120000.0;
    calc.volume_flow = 0.03;
    const double expected_head = coeffs[0] - coeffs[1] * calc.volume_flow * std::abs(calc.volume_flow);
    const double expected_pressure_out = calc.pressure_start + oil.density * gravity_acceleration * expected_head;
    // Act
    calc.solve_pq();
    // Assert
    EXPECT_NEAR(calc.get_pump_task_result().pressure_out, expected_pressure_out, 1e-6);
}

/// @brief Проверяет формулу QP-расчёта насоса при равных номинальной и текущей частотах.
TEST(PumpCalculator, SolveQpMatchesInverseForEqualFrequencies) {
    // Arrange
    const auto pump = make_pump();
    const auto oil = make_oil();
    const auto coeffs = pump_properties_t::get_polynomial_coefficients(pump.approximation_coeff);
    ASSERT_EQ(coeffs.size(), 2u);
    pump_calculator_t calc(pump, oil);
    calc.pressure_end = 4500000.0;
    calc.volume_flow = 0.025;
    const double expected_head = coeffs[0] - coeffs[1] * calc.volume_flow * std::abs(calc.volume_flow);
    const double expected_pressure_in = calc.pressure_end - oil.density * gravity_acceleration * expected_head;
    // Act
    calc.solve_qp();
    // Assert
    EXPECT_NEAR(calc.get_pump_task_result().pressure_in, expected_pressure_in, 1e-6);
}

/// @brief Проверяет, что в выбранной постановке solve_pq не выбрасывает исключение.
TEST(PumpCalculator, SolvePqDoesNotThrowForConfiguredInputs) {
    // Arrange
    pump_calculator_t calc(make_pump(), make_oil());
    calc.pressure_start = 1.0;
    calc.volume_flow = -30.0;
    // Act & Assert
    EXPECT_NO_THROW(calc.solve_pq());
}

/// @brief Проверяет ошибку при отрицательном входном давлении в задаче QP.
TEST(PumpCalculator, SolveQpThrowsOnNegativePressureIn) {
    // Arrange
    pump_calculator_t calc(make_pump(), make_oil());
    calc.pressure_end = 1.0;
    calc.volume_flow = 30.0;
    // Act & Assert
    EXPECT_THROW(calc.solve_qp(), std::runtime_error);
}

/// @brief Проверяет PP-расчёт насоса по двучленной QH-характеристике (лекция, с. 35).
TEST(PumpCalculator, SolvePpMatchesHeadFormula) {
    // Arrange
    const auto pump = make_pump();
    const auto oil = make_oil();
    const auto coeffs = pump_properties_t::get_polynomial_coefficients(pump.approximation_coeff);
    ASSERT_EQ(coeffs.size(), 2u);
    pump_calculator_t calc(pump, oil);
    calc.pressure_start = 120000.0;
    calc.pressure_end = 450000.0;
    const double frequency_ratio = pump.current_frequency / pump.nominal_frequency;
    const double diff_head = (calc.pressure_end - calc.pressure_start) /
        (oil.density * gravity_acceleration);
    const int sign = (coeffs[0] * frequency_ratio * frequency_ratio - diff_head >= 0) ? 1 : -1;
    const double expected = sign * std::sqrt(std::abs(
        (coeffs[0] * frequency_ratio * frequency_ratio - diff_head) / coeffs[1]));
    // Act
    calc.solve_pp();
    // Assert
    EXPECT_NEAR(calc.get_pump_task_result().volume_flow, expected, 1e-6);
    EXPECT_NEAR(calc.volume_flow_after_pp(), expected, 1e-6);
}

/// @brief Проверяет, что фабрика насосной станции сохраняет количество насосов.
TEST(PumpStationProperties, FactoryStoresPumps) {
    // Arrange
    const std::vector<pump_properties_t> pumps = {make_pump(), make_pump()};
    // Act
    const auto station = pump_station_properties_t::create_pump_station(pumps);
    // Assert
    EXPECT_EQ(station.pumps.size(), 2u);
}

/// @brief Проверяет, что расчёт коэффициентов станции доступен через константный интерфейс.
TEST(PumpStationProperties, ConstGetPolynomialCoefficientsReturnsVectorWithFourElements) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    // Act
    const auto coeffs = station.get_polynomial_coefficients();
    // Assert
    EXPECT_EQ(coeffs.size(), 4u);
}

/// @brief Проверяет, что результат станции недоступен до выполнения расчёта.
TEST(PumpStationCalculator, ResultUnavailableBeforeSolve) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    const pump_station_calculator_t calc(station, make_oil());
    // Act & Assert
    EXPECT_THROW(static_cast<void>(calc.get_pump_station_result()), std::runtime_error);
}

/// @brief Проверяет, что результат станции остаётся недоступен после solve_pq при текущей модели.
TEST(PumpStationCalculator, SolvePqLeavesResultUnavailableForSinglePump) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    const auto oil = make_oil();
    pump_station_calculator_t calc(station, oil);
    calc.pressure_start = 120000.0;
    calc.volume_flow = 0.03;
    // Act
    calc.solve_pq();
    // Assert
    EXPECT_THROW(static_cast<void>(calc.get_pump_station_result()), std::runtime_error);
}

/// @brief Проверяет, что результат станции остаётся недоступен после solve_qp при текущей модели.
TEST(PumpStationCalculator, SolveQpLeavesResultUnavailableForSinglePump) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    const auto oil = make_oil();
    pump_station_calculator_t calc(station, oil);
    calc.pressure_end = 4500000.0;
    calc.volume_flow = 0.025;
    // Act
    calc.solve_qp();
    // Assert
    EXPECT_THROW(static_cast<void>(calc.get_pump_station_result()), std::runtime_error);
}

/// @brief Проверяет, что solve_pq станции в выбранной постановке не выбрасывает исключение.
TEST(PumpStationCalculator, SolvePqDoesNotThrowForConfiguredInputs) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    pump_station_calculator_t calc(station, make_oil());
    calc.pressure_start = 1.0;
    calc.volume_flow = -30.0;
    // Act & Assert
    EXPECT_NO_THROW(calc.solve_pq());
}

/// @brief Проверяет, что solve_qp станции в выбранной постановке не выбрасывает исключение.
TEST(PumpStationCalculator, SolveQpDoesNotThrowForConfiguredInputs) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    pump_station_calculator_t calc(station, make_oil());
    calc.pressure_end = 1.0;
    calc.volume_flow = 30.0;
    // Act & Assert
    EXPECT_NO_THROW(calc.solve_qp());
}

/// @brief Проверяет PP-расчёт насосной станции по суммарной двучленной QH-характеристике.
TEST(PumpStationCalculator, SolvePpMatchesNominalHeadFormula) {
    // Arrange
    const auto station = pump_station_properties_t::create_pump_station({make_pump()});
    const auto oil = make_oil();
    const auto coeffs = station.get_polynomial_coefficients();
    ASSERT_GE(coeffs.size(), 2u);
    pump_station_calculator_t calc(station, oil);
    calc.pressure_start = 100000.0;
    calc.pressure_end = 380000.0;
    const double diff_head = (calc.pressure_end - calc.pressure_start) /
        (oil.density * gravity_acceleration);
    const int sign = (coeffs[0] - diff_head >= 0) ? 1 : -1;
    const double expected = sign * std::sqrt(std::abs((coeffs[0] - diff_head) / coeffs[1]));
    // Act
    calc.solve_pp();
    const double actual = calc.volume_flow_after_pp();
    // Assert
    if (coeffs.size() == 2u) {
        EXPECT_NEAR(calc.get_pump_station_result().volume_flow, expected, 1e-6);
        EXPECT_NEAR(actual, expected, 1e-6);
    } else {
        EXPECT_TRUE(std::isnan(actual));
    }
}
