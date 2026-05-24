/// @brief TZ46: тепловой прогрев трубы (RC-модель, FTCS). Данные примера — analitic.md.

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <numbers>

#include "pipe_thermal.h"

using namespace thermal_struct;

namespace {

constexpr double kRelTol = 0.02;

// Геометрия и границы (пример TZ46).
constexpr double kTz46InnerDiameter = 0.5;
constexpr double kTz46OuterDiameter = 0.6;
constexpr double kTz46Length = 1000.0;
constexpr double kTz46SupplyTemperature = 80.0;
constexpr double kTz46InitialTemperature = 20.0;
constexpr double kTz46AmbientTemperature = 20.0;
constexpr double kTz46ExternalHtc = 10.0;

// Вода ~20 °C.
constexpr double kWaterAt20Density = 998.2;
constexpr double kWaterAt20SpecificHeat = 4182.0;
constexpr double kWaterAt20Conductivity = 0.599;
constexpr double kWaterAt20KinematicViscosity = 1.006e-6;
constexpr double kWaterAt20Expansion = 2.07e-4;

// Вода ~50 °C (ветка естественной конвекции).
constexpr double kWaterAt50Density = 988.0;
constexpr double kWaterAt50SpecificHeat = 4181.0;
constexpr double kWaterAt50Conductivity = 0.640;
constexpr double kWaterAt50KinematicViscosity = 5.80e-7;
constexpr double kWaterAt50Expansion = 3.87e-4;

// Сталь AISI 304.
constexpr double kStainlessDensity = 7900.0;
constexpr double kStainlessSpecificHeat = 500.0;
constexpr double kStainlessConductivity = 16.0;

// Nu и эталоны из analitic.md.
constexpr double kNuCorrelationC = 0.13;
constexpr double kNuCorrelationN = 0.25;
constexpr double kDeltaTChar = 30.0;
constexpr double kWarmupFraction = 0.95;
constexpr double kExpectedRTotal = 0.586;
constexpr double kExpectedTauCond = 6.806e5;
constexpr double kExpectedT95Cond = 2.039e6;
constexpr double kExpectedBiot = 0.03125;
constexpr double kExpectedTauConv = 7.06e4;
constexpr double kExpectedTauRatio = 0.104;

/// @brief Свойства воды при 20 °C.
fluid_properties_t make_water_at_20c() {
    fluid_properties_t fluid;
    fluid.density = kWaterAt20Density;
    fluid.specific_heat = kWaterAt20SpecificHeat;
    fluid.thermal_conductivity = kWaterAt20Conductivity;
    fluid.kinematic_viscosity = kWaterAt20KinematicViscosity;
    fluid.volumetric_expansion = kWaterAt20Expansion;
    return fluid;
}

/// @brief Свойства воды при ~50 °C.
fluid_properties_t make_water_at_50c() {
    fluid_properties_t fluid;
    fluid.density = kWaterAt50Density;
    fluid.specific_heat = kWaterAt50SpecificHeat;
    fluid.thermal_conductivity = kWaterAt50Conductivity;
    fluid.kinematic_viscosity = kWaterAt50KinematicViscosity;
    fluid.volumetric_expansion = kWaterAt50Expansion;
    return fluid;
}

/// @brief Стенка из нержавеющей стали.
wall_material_properties_t make_stainless_steel() {
    wall_material_properties_t wall;
    wall.density = kStainlessDensity;
    wall.specific_heat = kStainlessSpecificHeat;
    wall.thermal_conductivity = kStainlessConductivity;
    return wall;
}

/// @brief Температуры и alpha_ext для примера TZ46.
thermal_boundary_t make_tz46_boundary() {
    thermal_boundary_t boundary;
    boundary.supply_temperature = kTz46SupplyTemperature;
    boundary.initial_temperature = kTz46InitialTemperature;
    boundary.ambient_temperature = kTz46AmbientTemperature;
    boundary.external_htc = kTz46ExternalHtc;
    return boundary;
}

/// @brief Калькулятор с геометрией и границами TZ46.
pipe_thermal_calculator_t make_tz46_calculator(std::unique_ptr<internal_htc_model_t> htc) {
    auto geometry = pipe_thermal_geometry_t::create(kTz46InnerDiameter, kTz46OuterDiameter,
                                                    kTz46Length);
    return pipe_thermal_calculator_t(std::move(geometry), make_tz46_boundary(),
                                     make_water_at_20c(), make_stainless_steel(),
                                     std::move(htc));
}

} // namespace

/// @brief R_wall и R_Σ по формулам task.md.
TEST(Tz46Resistances, WallAndTotalMatchFormulas) {
    // Arrange
    auto calc = make_tz46_calculator(std::make_unique<stagnant_conduction_htc_t>());
    const double r_wall_expected = std::log(kTz46OuterDiameter / kTz46InnerDiameter) /
                                   (2.0 * std::numbers::pi * kStainlessConductivity);

    // Act
    const auto r = calc.compute_resistances_per_meter();

    // Assert
    EXPECT_NEAR(r.r_wall, r_wall_expected, 1e-6);
    EXPECT_NEAR(r.r_total, r.r_int + r.r_wall + r.r_ext, 1e-9);
    EXPECT_GT(r.r_int, r.r_wall);
}

/// @brief τ = R_Σ·C_th и t_0,95 ≈ 3τ.
TEST(Tz46TimeConstant, TauEqualsRTimesC) {
    // Arrange
    auto calc = make_tz46_calculator(std::make_unique<stagnant_conduction_htc_t>());

    // Act
    const auto r = calc.compute_resistances_per_meter();
    const auto c = calc.compute_capacity_per_meter();
    const double tau = calc.compute_time_constant();
    const auto t95 = calc.time_to_fraction(kWarmupFraction);

    // Assert
    EXPECT_NEAR(tau, r.r_total * c.capacity_per_meter, 1e-6);
    EXPECT_NEAR(t95.time_to_fraction, -tau * std::log(1.0 - kWarmupFraction), 1e-6);
    EXPECT_NEAR(t95.time_to_fraction / tau, 3.0, 0.05);
}

/// @brief Естественная конвекция уменьшает τ.
TEST(Tz46ConvectionRatio, ConvectionFasterThanConduction) {
    // Arrange
    auto calc = make_tz46_calculator(std::make_unique<stagnant_conduction_htc_t>());

    // Act
    const auto cmp = calc.compare_conduction_vs_convection(
        kNuCorrelationC, kNuCorrelationN, kDeltaTChar, kWarmupFraction, make_water_at_50c());

    // Assert
    EXPECT_LT(cmp.convection.time_constant, cmp.conduction.time_constant);
    EXPECT_LT(cmp.tau_ratio, 1.0);
    EXPECT_GT(cmp.tau_ratio, 0.05);
}

/// @brief Сверка с analitic.md (допуск 2 %).
TEST(Tz46AnalyticExample, MatchesDocumentedValues) {
    // Arrange
    auto calc = make_tz46_calculator(std::make_unique<stagnant_conduction_htc_t>());

    // Act
    const auto r = calc.compute_resistances_per_meter();
    const double tau = calc.compute_time_constant();
    const auto t95 = calc.time_to_fraction(kWarmupFraction);
    const auto cmp = calc.compare_conduction_vs_convection(
        kNuCorrelationC, kNuCorrelationN, kDeltaTChar, kWarmupFraction, make_water_at_50c());

    // Assert
    EXPECT_NEAR(r.r_total, kExpectedRTotal, kRelTol * kExpectedRTotal);
    EXPECT_NEAR(tau, kExpectedTauCond, kRelTol * kExpectedTauCond);
    EXPECT_NEAR(t95.time_to_fraction, kExpectedT95Cond, kRelTol * kExpectedT95Cond);
    EXPECT_NEAR(r.biot, kExpectedBiot, 1e-4);
    EXPECT_NEAR(cmp.convection.time_constant, kExpectedTauConv, kRelTol * kExpectedTauConv);
    EXPECT_NEAR(cmp.tau_ratio, kExpectedTauRatio, kRelTol * kExpectedTauRatio);
}

/// @brief Для L = 1 км рекомендуется циркуляционный прогрев.
TEST(Tz46CirculationAdvice, RecommendsForLongPipe) {
    // Arrange
    auto calc = make_tz46_calculator(std::make_unique<stagnant_conduction_htc_t>());

    // Act
    const auto advice = calc.circulation_advice();

    // Assert
    EXPECT_EQ(advice.recommendation, circulation_recommendation_t::recommended);
}

/// @brief Дымовой тест FTCS: устойчивый шаг, рост температуры.
TEST(Tz46FtcsSmoke, StableStepAndHeatingAtInlet) {
    // Arrange
    constexpr double kFtcsPipeLength = 200.0;
    constexpr std::size_t kFtcsNodes = 51;
    constexpr double kFtcsTimeStep = 3600.0;
    constexpr double kFtcsMaxTime = 5.0e6;
    constexpr double kFtcsTargetFraction = 0.5;

    auto geometry =
        pipe_thermal_geometry_t::create(kTz46InnerDiameter, kTz46OuterDiameter, kFtcsPipeLength);
    auto boundary = make_tz46_boundary();
    auto fluid = make_water_at_20c();

    pipe_thermal_calculator_t rc_calc(geometry, boundary, fluid, make_stainless_steel(),
                                     std::make_unique<stagnant_conduction_htc_t>());
    const double u = 1.0 / rc_calc.compute_resistances_per_meter().r_total;

    ftcs_settings_t settings;
    settings.nodes = kFtcsNodes;
    settings.time_step = kFtcsTimeStep;
    settings.max_time = kFtcsMaxTime;
    settings.target_fraction = kFtcsTargetFraction;
    settings.stop = ftcs_stop_criterion_t::mean_temperature;

    axial_conduction_solver_t solver(geometry, boundary, fluid, u, settings);

    // Act
    const auto result = solver.run_until_fraction();

    // Assert
    EXPECT_GT(result.steps, 0u);
    EXPECT_GT(result.final_temperature, boundary.initial_temperature);
}
