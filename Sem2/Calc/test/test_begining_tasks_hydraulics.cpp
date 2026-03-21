#include "begining_tasks_hydraulics.h"


// --- Минимальные тесты для функций из заголовка (конверсии/расчёты/профили) ---

/// @brief Проверка l_si: переводит длину из км в м.
TEST(HydraulicsConversion, ConvertsKmToM) {
    // Arrange
    double l_km = 1.0;
    double expected_m = 1000.0;
    // Act
    double result = length_si(l_km);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_m);
}

/// @brief Проверка d_si: переводит диаметр из мм в м.
TEST(HydraulicsConversion, ConvertsMmToM) {
    // Arrange
    double d_mm = 1000.0;
    double expected_m = 1.0;
    // Act
    double result = diameter_si(d_mm);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_m);
}

/// @brief Проверка p_si: переводит давление из кПа в Па.
TEST(HydraulicsConversion, ConvertsKPaToPa) {
    // Arrange
    double p = 250.0;
    std::string unit = "кПа";
    double expected_Pa = 250000.0;
    // Act
    double result = pressure_si(p, unit);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_Pa);
}

/// @brief Проверка dynamic_viscosity_si: переводит сП в Па·с.
TEST(HydraulicsConversion, ConvertsCpToPaS) {
    // Arrange
    double mu_cP = 0.62;
    double expected_Pa_s = 0.0062;
    // Act
    double result = dynamic_viscosity_si(mu_cP);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_Pa_s);
}

/// @brief Проверка kinematic_viscosity_si: переводит сСт в м^2/с.
TEST(HydraulicsConversion, ConvertsCstToM2s) {
    // Arrange
    double nu_cSt = 1.0;
    double expected_m2_s = 0.01;
    // Act
    double result = kinematic_viscosity_si(nu_cSt);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_m2_s);
}

/// @brief Проверка re_calc: рассчитывает число Рейнольдса.
TEST(HydraulicsCalc, CalculatesReynoldsNumber) {
    // Arrange
    double velocity = 1.0;
    double d = 1.0;
    double kinematic_viscosity = 0.1;
    double expected_Re = 10.0;
    // Act
    double result = reynolds_number_calc(velocity, d, kinematic_viscosity);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_Re);
}

/// @brief Проверка head_calc: рассчитывает полный напор.
TEST(HydraulicsCalc, CalculatesHead) {
    // Arrange
    double p = 0.0;
    double density = 870.0;
    double z = 5.0;
    double expected_H = 5.0;
    // Act
    double result = total_head_calc(p, density, z);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected_H);
}

/// @brief Проверка sign: возвращает `1`, если первое число больше второго.
TEST(HydraulicsCalc, ReturnsPositiveSignWhenFirstIsGreater) {
    // Arrange
    double a = 10.0;
    double b = 5.0;
    // Act
    int result = sign(a, b);
    // Assert
    EXPECT_EQ(result, 1);
}

/// @brief Проверка sign: возвращает `-1`, если первое число меньше второго.
TEST(HydraulicsCalc, ReturnsNegativeSignWhenFirstIsLess) {
    // Arrange
    double a = 5.0;
    double b = 10.0;
    // Act
    int result = sign(a, b);
    // Assert
    EXPECT_EQ(result, -1);
}

/// @brief Проверка epsilon: рассчитывает относительную шероховатость.
TEST(HydraulicsCalc, CalculatesRelativeRoughness) {
    // Arrange
    double delta = 1e-4;
    double d = 0.1;
    double expected = 1e-3;
    // Act
    double result = epsilon(delta, d);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected);
}

/// @brief Проверка lambda_calc: рассчитывает коэффициент сопротивления в ламинарном режиме.
TEST(HydraulicsCalc, CalculatesFrictionFactorLaminar) {
    // Arrange (ламинарный режим Re < 2320)
    double Re = 1000.0;
    double eps = 0.001;
    double expected = 64.0 / 1000.0;
    // Act
    double result = lambda_calc(Re, eps);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected);
}

/// @brief Проверка pipe_area_calc: рассчитывает площадь сечения трубы.
TEST(HydraulicsCalc, CalculatesPipeArea) {
    // Arrange
    double d = 2.0;
    double expected = M_PI * d * d / 4.0;
    // Act
    double result = pipe_area_calc(d);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected);
}

/// @brief Проверка kinematic_viscosity_calc: рассчитывает кинематическую вязкость.
TEST(HydraulicsCalc, CalculatesKinematicViscosity) {
    // Arrange
    double dynamic_viscosity = 0.87;
    double density = 870.0;
    double expected = 0.001;
    // Act
    double result = kinematic_viscosity_calc(dynamic_viscosity, density);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected);
}

/// @brief Проверка q_calc: рассчитывает объёмный расход по массовому.
TEST(HydraulicsCalc, CalculatesVolumeFlowRate) {
    // Arrange
    double G = 0.87;
    double density = 870.0;
    double expected = 0.001;
    // Act
    double result = q_calc(G, density);
    // Assert
    EXPECT_DOUBLE_EQ(result, expected);
}

// --- Задачи вычисления входных/массовых параметров ---

/// @brief Проверка qp_task: вычисляет давление на входе для известного массового расхода.
TEST(HydraulicsTasks, CalculatesEntryPressure) {
    // Arrange
    double l = 450;
    double d = diameter_si(50);
    double z1 = 18;
    double z2 = 24;
    double p2 = pressure_si(250, "кПа");
    double G = 0.62;
    double density = 870;
    double dynamic_viscosity = dynamic_viscosity_si(0.62);
    double p1_calc = 302836;
    // Act
    double p1 = qp_task(l, d, z1, z2, p2, G, density, dynamic_viscosity);
    // Assert
    ASSERT_NEAR(p1, p1_calc, 1);
}

/// @brief Проверка pp_task: вычисляет массовый расход по заданным давлениям.
TEST(HydraulicsTasks, CalculatesMassFlowRate) {
    // Arrange
    double l = length_si(2.6);
    double d = diameter_si(200);
    double z1 = 52;
    double z2 = 59;
    double p1 = pressure_si(4.1, "кгс/см^2");
    double p2 = pressure_si(1.6, "кгс/см^2");
    double density = 735;
    double kinematic_viscosity = kinematic_viscosity_si(0.68);
    double G_calc = 36;
    // Act
    double G = pp_task(l, d, z1, z2, p1, p2, density, kinematic_viscosity);
    // Assert
    ASSERT_NEAR(G, G_calc, 1);
}

// --- Профили вдоль трассы ---

/// @brief Проверка pressure_profile/write_pressure_profile_csv: создаёт CSV-файл профиля давления.
TEST(HydraulicsProfiles, CreatesPressureProfileCsvFile) {
    // Arrange
    double length_m = length_si(2.6);
    double elevation_start_m = 52;
    (void)elevation_start_m; // требуется только для согласованности формата постановки задачи
    double elevation_end_m = 59;
    (void)elevation_end_m;
    double pressure_start_pa = pressure_si(4.1, "кгс/см^2");
    double pressure_end_pa = pressure_si(1.6, "кгс/см^2");
    int step_intervals = 20;
    std::string dir = std::string(CALC_SOURCE_DIR) + "/documents";
    std::filesystem::create_directories(dir);
    std::string filepath = dir + "/p_profile.csv";
    std::vector<std::vector<double>> pressure_prof = pressure_profile(pressure_start_pa, pressure_end_pa, length_m, step_intervals);

    // Act
    bool wr_file = write_pressure_profile_csv(filepath, pressure_prof, step_intervals);

    // Assert
    ASSERT_TRUE(wr_file);
}

/// @brief Проверка head_profile/write_head_profile_csv: создаёт CSV-файл профиля полного напора.
TEST(HydraulicsProfiles, CreatesHeadProfileCsvFile) {
    // Arrange
    double length_m = length_si(2.6);
    double elevation_start_m = 52;
    double elevation_end_m = 59;
    double pressure_start_pa = pressure_si(4.1, "кгс/см^2");
    double pressure_end_pa = pressure_si(1.6, "кгс/см^2");
    double density_kg_m3 = 735;
    int step_intervals = 20;
    std::string dir = std::string(CALC_SOURCE_DIR) + "/documents";
    std::filesystem::create_directories(dir);
    std::string filepath = dir + "/h_profile.csv";
    std::vector<std::vector<double>> head_prof = head_profile(pressure_start_pa, pressure_end_pa, length_m, step_intervals,
                                                                 density_kg_m3, elevation_start_m, elevation_end_m);

    // Act
    bool wr_file = write_head_profile_csv(filepath, head_prof, step_intervals);

    // Assert
    ASSERT_TRUE(wr_file);
}