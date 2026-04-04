#include "begining_tasks_hydraulics.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

#include "gtest/gtest.h"

using namespace hydraulics_struct;

namespace {

/// Синхронизирует копии геометрии и жидкости в `pipe_task_result` с полями калькулятора (так устроены solve_pq / solve_pp).
pipe_calculator_t make_calculator(pipe_properties_t pipe, oil_properties_t oil) {
    pipe_calculator_t calc;
    calc.pipe_properties = pipe;
    calc.oil_properties = oil;
    calc.pipe_task_result.pipe_properties = pipe;
    calc.pipe_task_result.oil_properties = oil;
    return calc;
}

pipe_calculator_t make_demo_calculator() {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.1, 10.0, 10.0, 15.0);
    props.pressure_start = 300000.0;
    props.pressure_end = 100000.0;
    props.mass_flow = 10.0;
    oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5, 850.0 * 1e-5);
    return make_calculator(props, oil);
}

/// Равномерная сетка вдоль трубы: `steps` — число интервалов (`steps + 1` точка), как в прежнем API pressure_profile(calc, steps).
pipe_profile_t linear_profile(const pipe_properties_t& props, int steps) {
    if (steps <= 0) {
        return pipe_profile_t::create_profile({}, {});
    }
    const int n = steps + 1;
    std::vector<double> x;
    std::vector<double> z;
    x.reserve(static_cast<size_t>(n));
    z.reserve(static_cast<size_t>(n));
    const double L = props.length;
    const double z0 = props.start_elevation;
    const double z1 = props.end_elevation;
    for (int i = 0; i < n; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(steps);
        x.push_back(t * L);
        z.push_back(z0 + t * (z1 - z0));
    }
    return pipe_profile_t::create_profile(std::move(x), std::move(z));
}

}  // namespace

// --- Неинициализированные структуры ---

TEST(HydraulicsStruct, UninitializedDefaultsYieldNaN) {
    pipe_properties_t props;
    EXPECT_TRUE(std::isnan(props.length));
    EXPECT_TRUE(std::isnan(props.inner_diameter));
    EXPECT_TRUE(std::isnan(props.wall_thickness));
    EXPECT_TRUE(std::isnan(props.pressure_start));
    EXPECT_TRUE(std::isnan(props.pressure_end));
    EXPECT_TRUE(std::isnan(props.get_relative_roughness()));
    EXPECT_TRUE(std::isnan(props.get_outer_diameter()));
    EXPECT_TRUE(std::isnan(props.get_pipe_area()));

    oil_properties_t oil;
    EXPECT_TRUE(std::isnan(oil.get_dynamic_viscosity()));
    EXPECT_TRUE(std::isnan(oil.get_kinematic_viscosity()));

    pipe_task_result_t result;
    EXPECT_TRUE(std::isnan(result.get_reynolds_number()));
    EXPECT_TRUE(std::isnan(result.get_start_total_head()));
    EXPECT_TRUE(std::isnan(result.get_end_total_head()));
    EXPECT_EQ(result.sign_difference(), 0);
    EXPECT_TRUE(std::isnan(result.get_hydraulic_resistance_coefficient()));
    EXPECT_TRUE(std::isnan(result.get_volume_flow_rate()));
}

// --- Фабрики ---

TEST(HydraulicsStruct, FactoryPipeAndOilAndCalculator) {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.5, 100.0, 50.0, 50.0);
    const double rho = 850.0;
    const double nu = 1e-5;
    oil_properties_t oil = oil_properties_t::create_oil(rho, nu, rho * nu);

    pipe_calculator_t calc = make_calculator(props, oil);

    EXPECT_DOUBLE_EQ(props.inner_diameter, 0.5);
    EXPECT_GT(props.get_pipe_area(), 0);
    EXPECT_DOUBLE_EQ(oil.density, rho);
    EXPECT_DOUBLE_EQ(oil.get_dynamic_viscosity(), rho * nu);
    EXPECT_DOUBLE_EQ(calc.pipe_properties.length, 1000.0);
}

// --- Перевод единиц ---

TEST(HydraulicsConversion, SiPrefixes) {
    EXPECT_DOUBLE_EQ(length_si(1.0, "км"), 1000.0);
    EXPECT_DOUBLE_EQ(diameter_si(1000.0, "мм"), 1.0);
    EXPECT_DOUBLE_EQ(pressure_si(250.0, "кПа"), 250000.0);
    EXPECT_DOUBLE_EQ(dynamic_viscosity_si(0.62, "сП"), 0.0062);
    EXPECT_DOUBLE_EQ(kinematic_viscosity_si(1.0, "сСт"), 1e-6);
    EXPECT_DOUBLE_EQ(dynamic_viscosity_si(1.0, "Па*с"), 1.0);
}

// --- Формулы через pipe_task_result_t ---

TEST(HydraulicsCalc, CoreFormulasViaTaskResult) {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(100.0, 0.1, 0.01, 0.0, 0.0);
    props.mass_flow = 870.0 * 1.0;  // Qv = 1 m³/s при ρ = 870
    props.pressure_start = 0.0;
    props.pressure_end = 0.0;
    oil_properties_t oil = oil_properties_t::create_oil(870.0, 1.0 / 870.0, 1.0);
    pipe_task_result_t r;
    r.pipe_properties = props;
    r.oil_properties = oil;

    EXPECT_DOUBLE_EQ(r.get_volume_flow_rate(), 1.0);
    const double v = r.get_velocity();
    EXPECT_FALSE(std::isnan(v));
    EXPECT_GT(v, 0.0);
    EXPECT_DOUBLE_EQ(r.get_reynolds_number(), v * props.inner_diameter / oil.kinematic_viscosity);

    EXPECT_EQ(r.sign_difference(), 1);  // оба напора 0 → разность 0, знак по условию >= 0
}

// --- Задачи solve_pq / solve_pp ---

TEST(HydraulicsTasks, SolvePqEntryPressure) {
    pipe_properties_t props;
    props.length = 450.0;
    props.inner_diameter = diameter_si(50.0, "мм");
    props.wall_thickness = 0.003;
    props.start_elevation = 18.0;
    props.end_elevation = 24.0;
    props.pressure_end = pressure_si(250.0, "кПа");
    props.mass_flow = 0.62;

    const double rho = 870.0;
    const double mu_si = dynamic_viscosity_si(0.62, "сП");
    const double nu_si = mu_si / rho;
    oil_properties_t oil = oil_properties_t::create_oil(rho, nu_si, rho * nu_si);
    pipe_calculator_t calc = make_calculator(props, oil);

    const double pressure_start = calc.solve_pq();

    ASSERT_FALSE(std::isnan(pressure_start));
    ASSERT_GT(pressure_start, props.pressure_end);
}

TEST(HydraulicsTasks, SolvePpMassFlow) {
    pipe_properties_t props;
    props.length = length_si(2.6, "км");
    props.inner_diameter = diameter_si(200.0, "мм");
    props.wall_thickness = 0.005;
    props.start_elevation = 52.0;
    props.end_elevation = 59.0;
    props.pressure_start = pressure_si(4.1, "кгс/см^2");
    props.pressure_end = pressure_si(1.6, "кгс/см^2");

    const double rho = 735.0;
    const double nu_si = kinematic_viscosity_si(0.68, "сСт");
    oil_properties_t oil = oil_properties_t::create_oil(rho, nu_si, rho * nu_si);
    pipe_calculator_t calc = make_calculator(props, oil);

    const double mass_flow = calc.solve_pp();

    ASSERT_FALSE(std::isnan(mass_flow));
    ASSERT_GT(mass_flow, 0.0);
}

// --- Профили и CSV ---

TEST(HydraulicsProfiles, PressureAndHeadAndCsv) {
    pipe_calculator_t calc = make_demo_calculator();
    constexpr int n = 10;
    pipe_profile_t prof = linear_profile(calc.pipe_properties, n);

    auto pprof = pressure_profile(calc, prof);
    ASSERT_EQ(pprof.size(), prof.get_point_count());
    for (size_t i = 1; i < pprof.size(); ++i) {
        EXPECT_GE(pprof[i][0], pprof[i - 1][0]);
    }

    auto hprof = head_profile(calc, prof);
    ASSERT_EQ(hprof.size(), pprof.size());
    for (const auto& pt : hprof) {
        ASSERT_EQ(pt.size(), 3u);
    }

    const std::string dir = std::string(CALC_SOURCE_DIR) + "/documents";
    std::filesystem::create_directories(dir);
    const std::string p_path = dir + "/p_profile.csv";
    const std::string h_path = dir + "/h_profile.csv";

    const bool ok_p = write_pressure_profile_csv(p_path, pprof);
    const bool ok_h = write_head_profile_csv(h_path, hprof);

    EXPECT_TRUE(ok_p);
    EXPECT_TRUE(ok_h);
    std::ifstream pf(p_path);
    std::string line;
    std::getline(pf, line);
    EXPECT_EQ(line, "x;z;p");
}

TEST(HydraulicsProfiles, EmptyProfileForBadSteps) {
    pipe_calculator_t calc = make_demo_calculator();
    pipe_profile_t empty0 = linear_profile(calc.pipe_properties, 0);
    pipe_profile_t empty_neg = linear_profile(calc.pipe_properties, -3);

    auto empty_p0 = pressure_profile(calc, empty0);
    auto empty_p_neg = pressure_profile(calc, empty_neg);
    auto empty_h = head_profile(calc, empty0);

    EXPECT_TRUE(empty_p0.empty());
    EXPECT_TRUE(empty_p_neg.empty());
    EXPECT_TRUE(empty_h.empty());
}

// --- Калькулятор ---

TEST(HydraulicsCalculator, SolvePqGlobal) {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.1, 10.0, 10.0, 15.0);
    props.pressure_end = 100000.0;
    props.mass_flow = 10.0;
    oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5, 850.0 * 1e-5);
    pipe_calculator_t calc = make_calculator(props, oil);

    const double g = calc.solve_pq();

    EXPECT_FALSE(std::isnan(g));
}

TEST(HydraulicsCalculator, SolvePpConstUsesOuterProperties) {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.1, 10.0, 10.0, 15.0);
    props.pressure_start = 300000.0;
    props.pressure_end = 100000.0;
    oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5, 850.0 * 1e-5);
    const pipe_calculator_t calc = make_calculator(props, oil);

    const double mass_flow = calc.solve_pp();

    EXPECT_FALSE(std::isnan(mass_flow));
}

TEST(HydraulicsCalculator, SolveReturnsNaNOnInvalidInput) {
    oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5, 850.0 * 1e-5);

    {
        pipe_properties_t bad_mass =
            pipe_properties_t::create_pipe(1000.0, 0.1, 10.0, 10.0, 50.0);
        bad_mass.pressure_end = 100000.0;
        bad_mass.mass_flow = std::numeric_limits<double>::quiet_NaN();
        pipe_calculator_t calc = make_calculator(bad_mass, oil);

        const double p = calc.solve_pq();
        EXPECT_TRUE(std::isnan(p));
    }

    {
        pipe_properties_t bad_d =
            pipe_properties_t::create_pipe(1000.0, 0.0, 10.0, 10.0, 50.0);
        bad_d.pressure_end = 100000.0;
        bad_d.mass_flow = 1.0;
        pipe_calculator_t calc = make_calculator(bad_d, oil);

        const double p = calc.solve_pq();
        EXPECT_TRUE(std::isnan(p));
    }

    {
        pipe_properties_t bad_pp =
            pipe_properties_t::create_pipe(1000.0, 0.1, 10.0, 10.0, 50.0);
        bad_pp.pressure_start = std::numeric_limits<double>::quiet_NaN();
        bad_pp.pressure_end = 100000.0;
        pipe_calculator_t calc = make_calculator(bad_pp, oil);

        const double m = calc.solve_pp();
        EXPECT_TRUE(std::isnan(m));
    }
}

// --- pipe_profile_t ---

TEST(PipeProfile, CreateAndAccessors) {
    pipe_profile_t p = pipe_profile_t::create_profile({0.0, 5.0, 10.0}, {1.0, 2.0, 3.0});
    EXPECT_EQ(p.get_point_count(), 3u);
    EXPECT_DOUBLE_EQ(p.get_length(), 10.0);

    pipe_profile_t d;
    EXPECT_EQ(d.get_point_count(), 0u);
    EXPECT_TRUE(std::isnan(d.get_length()));

    pipe_profile_t mismatch = pipe_profile_t::create_profile({0.0, 1.0}, {0.0});
    EXPECT_EQ(mismatch.get_point_count(), 0u);
}

// --- Краевые случаи ---

TEST(HydraulicsEdge, InvalidUnitsAndDomain) {
    EXPECT_TRUE(std::isnan(length_si(1.0, "фут")));
    EXPECT_TRUE(std::isnan(diameter_si(1.0, "дюйм")));
    EXPECT_TRUE(std::isnan(pressure_si(1.0, "psi")));
    EXPECT_TRUE(std::isnan(dynamic_viscosity_si(1.0, "cP")));

    pipe_properties_t zero_d =
        pipe_properties_t::create_pipe(100.0, 0.0, 0.01, 0.0, 0.0);
    pipe_task_result_t r0;
    r0.pipe_properties = zero_d;
    r0.oil_properties = oil_properties_t::create_oil(870.0, 1e-5, 870.0 * 1e-5);
    EXPECT_TRUE(std::isnan(r0.get_reynolds_number()));
    EXPECT_TRUE(std::isnan(r0.get_velocity()));

    pipe_task_result_t r_head;
    r_head.pipe_properties =
        pipe_properties_t::create_pipe(1.0, 0.1, 0.01, 0.0, 0.0);
    r_head.oil_properties = oil_properties_t::create_oil(0.0, 1e-5, 0.0);
    EXPECT_TRUE(std::isnan(r_head.get_start_total_head()));

    const double nan_v = std::numeric_limits<double>::quiet_NaN();
    pipe_task_result_t r_sign;
    r_sign.pipe_properties =
        pipe_properties_t::create_pipe(1.0, 0.1, 0.01, 0.0, 0.0);
    r_sign.pipe_properties.pressure_start = 1.0;
    r_sign.pipe_properties.pressure_end = nan_v;
    r_sign.oil_properties = oil_properties_t::create_oil(1000.0, 1e-6, 1.0);
    EXPECT_EQ(r_sign.sign_difference(), 0);

    EXPECT_TRUE(std::isnan(zero_d.get_relative_roughness()));

    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.5, 10.0, 10.0, 50.0);
    props.pressure_start = pressure_si(250.0, "кПа");
    EXPECT_DOUBLE_EQ(props.pressure_start, 250000.0);

    oil_properties_t oil_nan_rho;
    oil_nan_rho.density = std::numeric_limits<double>::quiet_NaN();
    oil_nan_rho.kinematic_viscosity = 1e-5;
    oil_nan_rho.dynamic_viscosity = 1e-2;
    EXPECT_TRUE(std::isnan(oil_nan_rho.get_dynamic_viscosity()));

    oil_properties_t oil_dyn;
    oil_dyn.density = 1000.0;
    oil_dyn.kinematic_viscosity = 1e-4;
    oil_dyn.dynamic_viscosity = 0.1;
    EXPECT_DOUBLE_EQ(oil_dyn.get_dynamic_viscosity(), 0.1);

    oil_properties_t oil_kin;
    oil_kin.density = 1000.0;
    oil_kin.kinematic_viscosity = 1e-4;
    oil_kin.dynamic_viscosity = 0.1;
    EXPECT_DOUBLE_EQ(oil_kin.get_kinematic_viscosity(), 1e-4);

    pipe_task_result_t r_lam;
    r_lam.pipe_properties =
        pipe_properties_t::create_pipe(10.0, 0.05, 0.002, 0.0, 0.0);
    r_lam.pipe_properties.mass_flow = 0.01;
    r_lam.oil_properties = oil_properties_t::create_oil(900.0, 1e-3, 0.9);
    const double re = r_lam.get_reynolds_number();
    ASSERT_FALSE(std::isnan(re));
    ASSERT_LT(re, 2320.0);
    EXPECT_DOUBLE_EQ(r_lam.get_hydraulic_resistance_coefficient(), 64.0 / re);

    pipe_task_result_t r_nan_re = r_lam;
    r_nan_re.pipe_properties.mass_flow = nan_v;
    // ṁ не задан (режим PP): λ оценивается по ε/d, а не по Re
    const double lam_pp = r_nan_re.get_hydraulic_resistance_coefficient();
    EXPECT_FALSE(std::isnan(lam_pp));
    EXPECT_GT(lam_pp, 0.0);

    EXPECT_TRUE(std::isnan(zero_d.get_pipe_area()));

    pipe_task_result_t r_q;
    r_q.pipe_properties = pipe_properties_t::create_pipe(1.0, 0.1, 0.01, 0.0, 0.0);
    r_q.pipe_properties.mass_flow = 1.0;
    r_q.oil_properties = oil_properties_t::create_oil(0.0, 1e-5, 0.0);
    EXPECT_TRUE(std::isnan(r_q.get_volume_flow_rate()));
}

TEST(HydraulicsEdge, TaskResultNaNPropagation) {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.1, 10.0, 10.0, 50.0);
    oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5, 850.0 * 1e-5);
    pipe_task_result_t r;
    r.pipe_properties = props;
    r.oil_properties = oil;
    r.pipe_properties.mass_flow = std::numeric_limits<double>::quiet_NaN();

    const double lambda = r.get_hydraulic_resistance_coefficient();

    EXPECT_FALSE(std::isnan(lambda));
    EXPECT_GT(lambda, 0.0);
}

// --- CSV ---

TEST(HydraulicsCsv, WriteFailsOnBadPath) {
    const std::vector<std::vector<double>> row = {{0.0, 1.0, 2.0}};
    const std::string bad = "Z:\\NonExistentDrive\\x.csv";

    const bool p_ok = write_pressure_profile_csv(bad, row);
    const bool h_ok = write_head_profile_csv(bad, row);

    EXPECT_FALSE(p_ok);
    EXPECT_FALSE(h_ok);
}

// --- Постановки из методички ---

TEST(FinalProblems, Task1_Solvent_PressureAtReactorAndProfiles) {
    pipe_properties_t props;
    props.length = 450.0;
    props.inner_diameter = diameter_si(50.0, "мм");
    props.wall_thickness = 0.003;
    props.start_elevation = 18.0;
    props.end_elevation = 24.0;
    props.mass_flow = 0.62;

    const double rho = 870.0;
    const double mu_si = dynamic_viscosity_si(0.62, "сП");
    const double nu_si = mu_si / rho;
    oil_properties_t oil = oil_properties_t::create_oil(rho, nu_si, rho * nu_si);

    props.pressure_end = 0.0;
    pipe_calculator_t calc_for_delta = make_calculator(props, oil);

    const double delta_p = calc_for_delta.solve_pq();
    const double p_storage_si = pressure_si(250.0, "кПа");
    const double p_reactor_inlet_si = p_storage_si - delta_p;

    ASSERT_FALSE(std::isnan(p_reactor_inlet_si));
    EXPECT_GT(p_reactor_inlet_si, 0.0);
    props.pressure_end = p_reactor_inlet_si;
    pipe_calculator_t calc = make_calculator(props, oil);
    EXPECT_NEAR(calc.solve_pq(), p_storage_si, 1.0);

    constexpr int steps = 50;
    pipe_profile_t prof = linear_profile(calc.pipe_properties, steps);
    auto pprof = pressure_profile(calc, prof);
    auto hprof = head_profile(calc, prof);

    const std::string dir = std::string(CALC_SOURCE_DIR) + "/documents";
    std::filesystem::create_directories(dir);
    const bool ok_p =
        write_pressure_profile_csv(dir + "/zadacha_1_p_profile.csv", pprof);
    const bool ok_h = write_head_profile_csv(dir + "/zadacha_1_h_profile.csv", hprof);

    ASSERT_EQ(pprof.size(), prof.get_point_count());
    ASSERT_EQ(hprof.size(), pprof.size());
    EXPECT_TRUE(ok_p);
    EXPECT_TRUE(ok_h);
}

TEST(FinalProblems, Task2_Gasoline_MassFlowAndProfiles) {
    pipe_properties_t props;
    props.length = length_si(2.6, "км");
    props.inner_diameter = diameter_si(200.0, "мм");
    props.wall_thickness = 0.005;
    props.start_elevation = 52.0;
    props.end_elevation = 59.0;
    props.pressure_start = pressure_si(4.1, "кгс/см^2");
    props.pressure_end = pressure_si(1.6, "кгс/см^2");

    const double rho = 735.0;
    const double nu_si = kinematic_viscosity_si(0.68, "сСт");
    oil_properties_t oil = oil_properties_t::create_oil(rho, nu_si, rho * nu_si);
    pipe_calculator_t calc = make_calculator(props, oil);

    const double mass_flow_kg_s = calc.solve_pp();

    ASSERT_FALSE(std::isnan(mass_flow_kg_s));
    ASSERT_GT(mass_flow_kg_s, 0.0);

    props.mass_flow = mass_flow_kg_s;
    calc = make_calculator(props, oil);
    (void)calc.solve_pq();

    constexpr int steps = 52;
    pipe_profile_t prof = linear_profile(calc.pipe_properties, steps);
    auto pprof = pressure_profile(calc, prof);
    auto hprof = head_profile(calc, prof);

    const std::string dir = std::string(CALC_SOURCE_DIR) + "/documents";
    std::filesystem::create_directories(dir);
    const bool ok_p =
        write_pressure_profile_csv(dir + "/zadacha_2_p_profile.csv", pprof);
    const bool ok_h = write_head_profile_csv(dir + "/zadacha_2_h_profile.csv", hprof);

    ASSERT_EQ(pprof.size(), prof.get_point_count());
    ASSERT_EQ(hprof.size(), pprof.size());
    EXPECT_TRUE(ok_p);
    EXPECT_TRUE(ok_h);
}

// --- Интеграция ---

TEST(HydraulicsIntegration, Pipeline) {
    pipe_properties_t props =
        pipe_properties_t::create_pipe(1000.0, 0.5, 10.0, 100.0, 50.0);
    props.pressure_start = 500000.0;
    props.pressure_end = 100000.0;
    props.mass_flow = 100.0;
    oil_properties_t oil = oil_properties_t::create_oil(850.0, 1e-5, 850.0 * 1e-5);
    pipe_calculator_t calc = make_calculator(props, oil);

    const double p_in = calc.solve_pq();
    const double m = calc.solve_pp();
    pipe_profile_t prof = linear_profile(calc.pipe_properties, 10);
    auto pp = pressure_profile(calc, prof);
    auto hp = head_profile(calc, prof);

    EXPECT_FALSE(std::isnan(p_in));
    EXPECT_FALSE(std::isnan(m));
    ASSERT_EQ(pp.size(), hp.size());
    for (size_t i = 0; i < pp.size(); ++i) {
        EXPECT_FALSE(std::isnan(pp[i][2]));
        EXPECT_FALSE(std::isnan(hp[i][2]));
    }
}
