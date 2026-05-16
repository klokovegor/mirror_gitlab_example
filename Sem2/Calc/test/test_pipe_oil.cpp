/// @brief Тесты единиц СИ, геометрии трубы, нефти, калькулятора трубы и экспорта CSV.

#include "hydraulic_chain.h"

#include "gtest/gtest.h"

using namespace hydraulics_struct;

/// @brief Проверяет перевод поддерживаемых единиц в СИ.
TEST(PipeOilUnits, ConvertsSupportedUnitsToSI) {
    // Arrange & Act & Assert
    EXPECT_DOUBLE_EQ(length_si(1.0, "км"), 1000.0);
    EXPECT_DOUBLE_EQ(diameter_si(250.0, "мм"), 0.25);
    EXPECT_DOUBLE_EQ(pressure_si(250.0, "кПа"), 250000.0);
    EXPECT_DOUBLE_EQ(kinematic_viscosity_si(2.0, "сСт"), 2e-6);
    EXPECT_DOUBLE_EQ(dynamic_viscosity_si(0.62, "сП"), 0.0062);
}

/// @brief Проверяет возврат NaN для неподдерживаемых единиц.
TEST(PipeOilUnits, ReturnsNaNForUnknownUnits) {
    // Arrange & Act & Assert
    EXPECT_TRUE(std::isnan(length_si(1.0, "yard")));
    EXPECT_TRUE(std::isnan(diameter_si(1.0, "inch")));
    EXPECT_TRUE(std::isnan(pressure_si(1.0, "psi")));
    EXPECT_TRUE(std::isnan(kinematic_viscosity_si(1.0, "cSt")));
    EXPECT_TRUE(std::isnan(dynamic_viscosity_si(1.0, "cP")));
}

/// @brief Проверяет, что значения в единицах по умолчанию не изменяются.
TEST(PipeOilUnits, UsesDefaultUnitsAsIdentity) {
    // Arrange & Act & Assert
    EXPECT_DOUBLE_EQ(length_si(12.5), 12.5);
    EXPECT_DOUBLE_EQ(diameter_si(0.42), 0.42);
    EXPECT_DOUBLE_EQ(pressure_si(101325.0), 101325.0);
    EXPECT_DOUBLE_EQ(kinematic_viscosity_si(1.5e-6), 1.5e-6);
    EXPECT_DOUBLE_EQ(dynamic_viscosity_si(0.003), 0.003);
}

/// @brief Проверяет дополнительные поддерживаемые преобразования единиц.
TEST(PipeOilUnits, SupportsAdditionalKnownUnits) {
    // Arrange & Act & Assert
    EXPECT_DOUBLE_EQ(length_si(25.0, "см"), 0.25);
    EXPECT_DOUBLE_EQ(pressure_si(1.0, "кгс/см^2"), 98066.5);
}

/// @brief Проверяет значения физических констант модуля.
TEST(PipeOilConstants, HaveExpectedValues) {
    // Arrange & Act & Assert
    EXPECT_DOUBLE_EQ(gravity_acceleration, 9.81);
    EXPECT_DOUBLE_EQ(equivalent_roughness, 1e-4);
}

/// @brief Проверяет фабричный метод профиля и геометрические аксессоры.
TEST(PipeProfile, FactoryAndGeometryAccessors) {
    // Arrange
    const pipe_profile_t profile = pipe_profile_t::create_profile({0.0, 5.0, 10.0}, {10.0, 11.0, 12.0});
    // Act
    const size_t n = profile.get_point_count();
    const double len0 = profile.get_length(0);
    const double len1 = profile.get_length(1);
    const double len2 = profile.get_length(2);
    // Assert
    EXPECT_EQ(n, 3u);
    EXPECT_DOUBLE_EQ(len0, 10.0);
    EXPECT_DOUBLE_EQ(len1, 5.0);
    EXPECT_DOUBLE_EQ(len2, 5.0);
}

/// @brief Проверяет валидацию согласованности данных профиля.
TEST(PipeProfile, CheckParametersValidatesShape) {
    // Arrange
    const pipe_profile_t valid = pipe_profile_t::create_profile({0.0, 1.0}, {0.0, 1.0});
    const pipe_profile_t empty = pipe_profile_t::create_profile({}, {});
    const pipe_profile_t mismatched = pipe_profile_t::create_profile({0.0, 1.0}, {0.0});
    // Act & Assert
    EXPECT_NO_THROW(valid.check_parameters());
    EXPECT_THROW(empty.check_parameters(), std::runtime_error);
    EXPECT_THROW(mismatched.check_parameters(), std::runtime_error);
}

/// @brief Проверяет фабрику параметров трубы и вычисляемые характеристики.
TEST(PipeProperties, FactoryAndDerivedValues) {
    // Arrange
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.2, 0.01);
    // Act
    const double rel_rough = pipe.get_relative_roughness();
    const double d_out = pipe.get_outer_diameter();
    const double area = pipe.get_pipe_area();
    // Assert
    EXPECT_DOUBLE_EQ(pipe.inner_diameter, 0.2);
    EXPECT_DOUBLE_EQ(pipe.wall_thickness, 0.01);
    EXPECT_NEAR(rel_rough, equivalent_roughness / 0.2, 1e-12);
    EXPECT_DOUBLE_EQ(d_out, 0.22);
    EXPECT_NEAR(area, std::numbers::pi * 0.2 * 0.2 / 4.0, 1e-12);
}

/// @brief Проверяет отклонение отрицательного внутреннего диаметра.
TEST(PipeProperties, CheckParametersRejectsNegativeDiameter) {
    // Arrange
    const pipe_properties_t bad = pipe_properties_t::create_pipe(-0.1, 0.01);
    // Act & Assert
    EXPECT_THROW(bad.check_parameters(), std::runtime_error);
}

/// @brief Проверяет NaN-поведение константных вычислительных методов трубы.
TEST(PipeProperties, DerivedConstMethodsReturnNaNForNaNInputs) {
    // Arrange
    pipe_properties_t pipe{};
    pipe.inner_diameter = std::numeric_limits<double>::quiet_NaN();
    pipe.wall_thickness = 0.01;
    // Act & Assert
    EXPECT_TRUE(std::isnan(pipe.get_relative_roughness()));
    EXPECT_TRUE(std::isnan(pipe.get_outer_diameter()));
    EXPECT_TRUE(std::isnan(pipe.get_pipe_area()));

    // Arrange
    pipe.inner_diameter = 0.2;
    pipe.wall_thickness = std::numeric_limits<double>::quiet_NaN();
    // Act & Assert
    EXPECT_TRUE(std::isnan(pipe.get_outer_diameter()));
}

/// @brief Проверяет фабрику свойств нефти и расчет кинематической вязкости.
TEST(OilProperties, FactoryAndKinematicFromDynamic) {
    // Arrange
    const oil_properties_t oil = oil_properties_t::create_oil(850.0, 1.2e-5);
    // Act
    const double nu_from_mu = oil_properties_t::calc_kinematic_by_dynamic(850.0, 0.0085);
    // Assert
    EXPECT_DOUBLE_EQ(oil.density, 850.0);
    EXPECT_DOUBLE_EQ(oil.kinematic_viscosity, 1.2e-5);
    EXPECT_TRUE(std::isnan(oil.dynamic_viscosity));
    EXPECT_NEAR(nu_from_mu, 1e-5, 1e-12);
}

/// @brief Проверяет возврат NaN при NaN-аргументах расчета вязкости.
TEST(OilProperties, KinematicFromDynamicReturnsNaNOnNaNInput) {
    // Arrange & Act & Assert
    EXPECT_TRUE(std::isnan(oil_properties_t::calc_kinematic_by_dynamic(
        std::numeric_limits<double>::quiet_NaN(), 0.01)));
    EXPECT_TRUE(std::isnan(oil_properties_t::calc_kinematic_by_dynamic(
        850.0, std::numeric_limits<double>::quiet_NaN())));
}

/// @brief Проверяет валидацию плотности и динамической вязкости нефти.
TEST(OilProperties, CheckParametersValidatesDensityAndDynamicViscosity) {
    // Arrange
    oil_properties_t valid{};
    valid.density = 850.0;
    valid.dynamic_viscosity = 0.007;
    oil_properties_t bad_density = valid;
    bad_density.density = 0.0;
    oil_properties_t bad_dynamic = valid;
    bad_dynamic.dynamic_viscosity = -0.001;
    // Act & Assert
    EXPECT_NO_THROW(valid.check_parameters());
    EXPECT_THROW(bad_density.check_parameters(), std::runtime_error);
    EXPECT_THROW(bad_dynamic.check_parameters(), std::runtime_error);
}

/// @brief Проверяет недоступность результата до вызова solve-методов.
TEST(PipeCalculator, ResultUnavailableBeforeSolve) {
    // Arrange
    const pipe_profile_t profile = pipe_profile_t::create_profile({0.0, 1.0}, {0.0, 0.0});
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.1, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5);
    const pipe_calculator_t calc(profile, pipe, oil);
    // Act & Assert
    EXPECT_THROW(static_cast<void>(calc.get_pipe_task_result()), std::runtime_error);
}

/// @brief Проверяет PP-расчёт при нулевом перепаде полных напоров.
TEST(PipeCalculator, SolvePpZeroFlowWhenHeadDifferenceIsZero) {
    // Arrange
    const pipe_profile_t profile = pipe_profile_t::create_profile({0.0, 1.0}, {0.0, 0.0});
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.1, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5);
    pipe_calculator_t calc(profile, pipe, oil);
    calc.pressure_start = 200000.0;
    calc.pressure_end = 200000.0;
    // Act & Assert
    ASSERT_NO_THROW(calc.solve_pp());
    EXPECT_NEAR(calc.volume_flow, 0.0, 1e-12);
    EXPECT_NEAR(calc.get_pipe_task_result().volume_flow, 0.0, 1e-12);
}

/// @brief Проверяет восстановление расхода PP после прямого расчёта PQ.
TEST(PipeCalculator, SolvePpRoundTripFromPq) {
    // Arrange
    const pipe_profile_t profile = pipe_profile_t::create_profile(
        {0.0, 50.0, 100.0, 150.0}, {10.0, 12.0, 15.0, 18.0});
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.2, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5);
    pipe_calculator_t calc(profile, pipe, oil);
    calc.pressure_start = 500000.0;
    calc.volume_flow = 0.04;
    calc.solve_pq();
    const double q_ref = calc.volume_flow;
    const double p_end = calc.get_pipe_task_result().pressure_profile.back();
    calc.volume_flow = std::numeric_limits<double>::quiet_NaN();
    calc.pressure_end = p_end;
    // Act & Assert
    ASSERT_NO_THROW(calc.solve_pp());
    EXPECT_NEAR(calc.volume_flow, q_ref, 1e-4);
    EXPECT_NEAR(calc.get_pipe_task_result().volume_flow, q_ref, 1e-4);
}

/// @brief Проверяет согласованность выходного давления PP с граничным условием.
TEST(PipeCalculator, SolvePpEndPressureMatchesBoundary) {
    // Arrange
    const pipe_profile_t profile = pipe_profile_t::create_profile(
        {0.0, 80.0, 160.0}, {5.0, 8.0, 11.0});
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(0.15, 0.005);
    const oil_properties_t oil = oil_properties_t::create_oil(870.0, 1.2e-5);
    pipe_calculator_t calc(profile, pipe, oil);
    calc.pressure_start = 400000.0;
    calc.pressure_end = 280000.0;
    // Act
    ASSERT_NO_THROW(calc.solve_pp());
    const double q_pp = calc.volume_flow;
    calc.volume_flow = q_pp;
    calc.solve_pq();
    // Assert
    EXPECT_NEAR(calc.get_pipe_task_result().pressure_profile.back(), calc.pressure_end, 1.0);
}

/// @brief Проверяет отказ записи CSV для пустых и некорректных путей.
TEST(PipeCsvWriters, ReturnFalseForEmptyAndInvalidPaths) {
    // Arrange & Act & Assert
    EXPECT_FALSE(write_head_profile_csv(""));
    EXPECT_FALSE(write_pressure_profile_csv(""));

#ifndef _WIN32
    EXPECT_FALSE(write_head_profile_csv("C:\\temp\\head_profile.csv"));
    EXPECT_FALSE(write_pressure_profile_csv("C:\\temp\\pressure_profile.csv"));
#endif
}

/// @brief Сохраняет профили в /profiles рядом с exe для сценария QP.
TEST(PipeTaskCsv, WritesProfilesToExeRelativeProfilesDir) {
    // Arrange
    const pipe_profile_t profile = pipe_profile_t::create_profile(
        {0.0, 50.0, 100.0, 150.0, 200.0, 250.0,
            300.0, 350.0, 400.0, length_si(450.0)},
        {18.0, 19.0, 20.0, 21.0, 22.0, 22.0, 23.0,
            23.0, 24.0, 24.0}
    );
    const pipe_properties_t pipe = pipe_properties_t::create_pipe(diameter_si(50.0, "мм"), 0.003);
    const oil_properties_t oil = oil_properties_t::create_oil(
        870.0,
        oil_properties_t::calc_kinematic_by_dynamic(870.0, dynamic_viscosity_si(0.62, "сП"))
    );
    ASSERT_NO_THROW(profile.check_parameters());
    ASSERT_NO_THROW(pipe.check_parameters());
    ASSERT_TRUE(std::isfinite(oil.kinematic_viscosity));
    pipe_calculator_t calc(profile, pipe, oil);
    calc.pressure_end = pressure_si(250.0, "кПа");
    calc.mass_flow = 0.62;
    const std::filesystem::path output_dir = std::filesystem::current_path() / "profiles";
    std::filesystem::create_directories(output_dir);
    const std::filesystem::path head_csv = output_dir / "h_profile_qp_1.csv";
    const std::filesystem::path pressure_csv = output_dir / "p_profile_qp_1.csv";
    // Act
    ASSERT_TRUE(write_head_profile_csv(head_csv.string()));
    ASSERT_TRUE(write_pressure_profile_csv(pressure_csv.string()));
    {
        std::ofstream out(head_csv, std::ios::app);
        ASSERT_TRUE(out.is_open());
        for (size_t i = 0; i < profile.get_point_count(); ++i) {
            const double pressure = calc.pressure_end;
            const double head = pressure / (oil.density * gravity_acceleration) + profile.elevations[i];
            out << profile.coordinates[i] << ';' << profile.elevations[i] << ';' << head << '\n';
        }
        ASSERT_TRUE(out.good());
    }
    {
        std::ofstream out(pressure_csv, std::ios::app);
        ASSERT_TRUE(out.is_open());
        for (size_t i = 0; i < profile.get_point_count(); ++i) {
            out << profile.coordinates[i] << ';' << profile.elevations[i] << ';' << calc.pressure_end << '\n';
        }
        ASSERT_TRUE(out.good());
    }
    // Assert
    ASSERT_TRUE(std::filesystem::exists(head_csv));
    ASSERT_TRUE(std::filesystem::exists(pressure_csv));
}
