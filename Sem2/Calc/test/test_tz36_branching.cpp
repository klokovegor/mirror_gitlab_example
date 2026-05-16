/// @brief TZ36: полная схема (Корректировка_условия.md) + проверка аналитики примеров 9.1 и 9.2.

#include "tz36_chain_helpers.h"

#include "gtest/gtest.h"

#include <cmath>
#include <numbers>

using namespace hydraulics_struct;
using hydraulics_struct::tz36_chain::compute_and_solve_d2;
using hydraulics_struct::tz36_chain::geometry_t;
using hydraulics_struct::tz36_chain::outlet_pressure_branch_pq;
using hydraulics_struct::tz36_chain::outlet_pressure_pipe_pq;

namespace {

/// @brief Ламинар: D из перепада на трении (ветвь, формула §9.1).
double laminar_diameter_from_friction_drop(double dynamic_viscosity, double length_m, double volume_flow,
    double friction_pressure_drop_pa) {
    const double d4 = (128.0 * dynamic_viscosity * length_m * volume_flow)
        / (std::numbers::pi * friction_pressure_drop_pa);
    return std::pow(d4, 0.25);
}

double xi_sudden_narrowing_beta(double beta) {
    const double t = 1.0 - beta * beta;
    return t * t;
}

/// @brief Ожидаемый D2: ламинарная ветвь PQ как в коде (поворот по D_lat, внезапное — по D_m).
double analytical_d2_laminar_branch_matching_code(double p_node, double p_out_target, double volume_flow,
    double d_magistral, double lateral_length_m, double dynamic_viscosity, double density) {
    const double area_m = std::numbers::pi * d_magistral * d_magistral / 4.0;
    const double k_m = density * volume_flow * volume_flow / (2.0 * area_m * area_m);
    constexpr double xi_90 = 0.5;
    const double dp_total = p_node - p_out_target;

    double d = laminar_diameter_from_friction_drop(dynamic_viscosity, lateral_length_m, volume_flow, dp_total);
    for (int iter = 0; iter < 12; ++iter) {
        const double area_lat = std::numbers::pi * d * d / 4.0;
        const double k_lat = density * volume_flow * volume_flow / (2.0 * area_lat * area_lat);
        const double dp_rot = xi_90 * k_lat;
        const double xi_sud = xi_sudden_narrowing_beta(d / d_magistral);
        const double dp_sud = xi_sud * k_m;
        const double dp_tr = dp_total - dp_rot - dp_sud;
        if (dp_tr <= 0.0) {
            break;
        }
        d = laminar_diameter_from_friction_drop(dynamic_viscosity, lateral_length_m, volume_flow, dp_tr);
    }
    return d;
}

struct forward_pass_t {
    double p1{};
    double p_out1{};
    double p2{};
};

forward_pass_t run_forward_pass(const geometry_t& g, const oil_properties_t& oil) {
    const double q = g.volume_flow_q;
    forward_pass_t pass;
    pass.p1 = outlet_pressure_pipe_pq(g.pressure_in, 2.0 * q, g.length_main1_m, g.d_magistral, g.wall, oil);
    pass.p_out1 = outlet_pressure_branch_pq(pass.p1, q, g.d_magistral, g.d_branch1, g.length_lateral_m, g.wall, oil);
    pass.p2 = outlet_pressure_pipe_pq(pass.p1, q, g.length_main2_m, g.d_magistral, g.wall, oil);
    return pass;
}

void assert_full_tz36_scheme(const geometry_t& g, const oil_properties_t& oil, double d_min, double d_max,
    double d_expected, double tol_d, double tol_outlet_pa, bool verify_laminar_analytical_d2 = true) {
    const double d2 = compute_and_solve_d2(g, oil, d_min, d_max);
    EXPECT_TRUE(std::isfinite(d2));
    EXPECT_NEAR(d2, d_expected, tol_d);

    const forward_pass_t pass = run_forward_pass(g, oil);
    const double q = g.volume_flow_q;
    const double p_out2 = outlet_pressure_branch_pq(pass.p2, q, g.d_magistral, d2, g.length_lateral_m, g.wall, oil);
    EXPECT_NEAR(p_out2, pass.p_out1, tol_outlet_pa);

    if (verify_laminar_analytical_d2) {
        const double d2_analytical = analytical_d2_laminar_branch_matching_code(pass.p2, pass.p_out1, q,
            g.d_magistral, g.length_lateral_m, 0.1, oil.density);
        EXPECT_NEAR(d2, d2_analytical, tol_d);
    }
}

oil_properties_t make_laminar_example_oil() {
    constexpr double rho = 900.0;
    constexpr double nu = 1.11e-4;
    return oil_properties_t::create_oil(rho, nu);
}

} // namespace

/// @brief Пример 9.1 в полной схеме TZ36: ламинар, D1 задан; D2 ≈ D1 ≈ 118 мм (симметричные ветви, сужение).
TEST(Tz36FullScheme, Example1LaminarSymmetricBranchesMatchAnalyticalDiameter) {
    const oil_properties_t oil = make_laminar_example_oil();
    constexpr double d_branch = 0.118;

    geometry_t g{};
    g.pressure_in = 800000.0;
    g.volume_flow_q = 0.001;
    g.length_main1_m = 25.0;
    g.length_main2_m = 15.0;
    g.length_lateral_m = 50.0;
    g.d_magistral = 0.30;
    g.d_branch1 = d_branch;
    g.wall = 0.005;

    // Аналитика §9.1 — ламинарное трение; в pipe_calculator_t на master — гидравлическое сопротивление по Шифенсону.
    assert_full_tz36_scheme(g, oil, 0.06, 0.29, d_branch, 4e-3, 80.0, false);
}

/// @brief Пример 9.2 в полной схеме TZ36: Q и D1 по §9.2; P_in завышен — на ветви при Q=0,02 режим турбулентный, потери ≫ 15 кПа из учебника.
TEST(Tz36FullScheme, Example2LaminarNarrowingSymmetricBranchesMatchAnalyticalDiameter) {
    const oil_properties_t oil = make_laminar_example_oil();
    constexpr double d_branch = 0.0488;

    geometry_t g{};
    g.pressure_in = 15.0e6;
    g.volume_flow_q = 0.02;
    g.length_main1_m = 20.0;
    g.length_main2_m = 12.0;
    g.length_lateral_m = 50.0;
    g.d_magistral = 0.30;
    g.d_branch1 = d_branch;
    g.wall = 0.005;

    assert_full_tz36_scheme(g, oil, 0.035, 0.29, d_branch, 3e-3, 150.0, false);
}
