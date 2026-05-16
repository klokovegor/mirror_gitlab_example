/// @brief Вспомогательные PQ-расчёты TZ36: несколько линейных `hydro_chain_model_t` на участки сети.

#pragma once

#include "hydraulic_chain.h"
#include "local_resistance.h"
#include "pipe_oil.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace hydraulics_struct::tz36_chain {

namespace detail {

/// @brief Диаметр боковой трубы совпадает с магистралью — внезапного сужения/расширения нет (ξ не задаётся).
inline bool lateral_equals_main_diameter(double d_magistral, double d_lateral) {
    const double scale = std::max({1e-9, std::abs(d_magistral), std::abs(d_lateral)});
    return std::abs(d_lateral - d_magistral) <= 1e-9 * scale;
}

/// @brief Вид внезапного перехода, если диаметры различаются (иначе не вызывать).
inline local_resistance_kind_t sudden_kind_when_different(double d_magistral, double d_lateral) {
    const double scale = std::max({1e-9, std::abs(d_magistral), std::abs(d_lateral)});
    const double tol = 1e-9 * scale;
    if (d_lateral + tol < d_magistral) {
        return local_resistance_kind_t::sudden_narrowing;
    }
    return local_resistance_kind_t::sudden_expansion;
}

inline local_resistance_properties_t make_sudden_local(double d_magistral, double d_lateral) {
    const local_resistance_kind_t kind = sudden_kind_when_different(d_magistral, d_lateral);
    return local_resistance_properties_t::create_local_resistance(0.0, kind, d_magistral, d_lateral);
}

/// @brief Интервал [d_min, d_max] не должен пересекать D_m: иначе смена вида перехода даёт разрыв невязки.
inline void assert_bisection_interval_one_side_of_main(double d_min, double d_max, double d_magistral) {
    const double scale = std::max({1e-9, std::abs(d_magistral), std::abs(d_min), std::abs(d_max)});
    const double tol = 1e-7 * scale;
    if (d_min < d_magistral - tol && d_max > d_magistral + tol) {
        throw std::runtime_error(
            "TZ36: подбор D2 — интервал не должен пересекать D_m (сужение слева, расширение справа); разбейте расчёт");
    }
}

} // namespace detail

inline oil_properties_t make_oil() {
    return oil_properties_t::create_oil(850.0, 1e-5);
}

/// @brief PQ: одна горизонтальная труба длины L, диаметр D.
inline double outlet_pressure_pipe_pq(double pressure_in, double volume_flow, double length_m, double inner_d,
    double wall, const oil_properties_t& oil) {
    chain_task_properties_t props = chain_task_properties_t::create_chain({type_of_obj_t::pipe});
    props.oil_properties = oil;
    props.pipe_properties.push_back(pipe_properties_t::create_pipe(inner_d, wall));
    props.pipe_profile.push_back(pipe_profile_t::create_profile({0.0, length_m}, {0.0, 0.0}));
    hydro_chain_model_t chain(props);
    chain.pressure_in = pressure_in;
    chain.volume_flow = volume_flow;
    chain.solve_pq();
    return chain.pressure_out;
}

/// @brief PQ: ветвь «поворот 90° + при необходимости внезапное сужение/расширение (D_lat ≠ D_m) + труба».
inline double outlet_pressure_branch_pq(double pressure_in, double volume_flow, double d_magistral, double d_lateral,
    double lateral_length_m, double wall, const oil_properties_t& oil) {
    const bool skip_sudden = detail::lateral_equals_main_diameter(d_magistral, d_lateral);
    chain_task_properties_t props = skip_sudden
        ? chain_task_properties_t::create_chain({type_of_obj_t::local_resistance, type_of_obj_t::pipe})
        : chain_task_properties_t::create_chain(
              {type_of_obj_t::local_resistance, type_of_obj_t::local_resistance, type_of_obj_t::pipe});
    props.oil_properties = oil;
    props.pipe_properties.push_back(pipe_properties_t::create_pipe(d_lateral, wall));
    props.pipe_profile.push_back(pipe_profile_t::create_profile({0.0, lateral_length_m}, {0.0, 0.0}));
    props.local_resistance_properties.push_back(local_resistance_properties_t::create_local_resistance(
        0.0, local_resistance_kind_t::rotation_pipe_90, d_lateral, d_lateral));
    if (!skip_sudden) {
        props.local_resistance_properties.push_back(detail::make_sudden_local(d_magistral, d_lateral));
    }
    hydro_chain_model_t chain(props);
    chain.pressure_in = pressure_in;
    chain.volume_flow = volume_flow;
    chain.solve_pq();
    return chain.pressure_out;
}

inline double residual_outlet_pressure(double p2, double p_out1, double q, double d_magistral, double d2,
    double lateral_length_m, double wall, const oil_properties_t& oil) {
    const double p_out2 = outlet_pressure_branch_pq(p2, q, d_magistral, d2, lateral_length_m, wall, oil);
    return p_out2 - p_out1;
}

/// @brief Подбор D2 дихотомией на [d_min, d_max] (весь интервал с одной стороны от D_m — только сужение или только расширение).
inline double solve_d2_bisection(double p2, double p_out1, double q, double d_magistral, double lateral_length_m,
    double wall, const oil_properties_t& oil, double d_min, double d_max, int max_iter, double pressure_tol_pa) {
    detail::assert_bisection_interval_one_side_of_main(d_min, d_max, d_magistral);
    const auto f = [&](double d) {
        return residual_outlet_pressure(p2, p_out1, q, d_magistral, d, lateral_length_m, wall, oil);
    };
    double lo = d_min;
    double hi = d_max;
    double f_lo = f(lo);
    double f_hi = f(hi);
    if (!std::isfinite(f_lo) || !std::isfinite(f_hi)) {
        throw std::runtime_error("TZ36: невязка на границе интервала не конечна");
    }
    constexpr int k_expand = 24;
    const double d_cap = std::max({0.05, d_magistral * 1.25, d_max * 1.25});
    for (int e = 0; e < k_expand && f_lo * f_hi > 0.0 && hi < d_cap; ++e) {
        hi = std::min(hi * 1.12 + 0.015, d_cap);
        f_hi = f(hi);
    }
    if (f_lo * f_hi > 0.0) {
        throw std::runtime_error("TZ36: нет смены знака невязки на интервале диаметра");
    }
    if (f_lo > 0.0) {
        std::swap(lo, hi);
        std::swap(f_lo, f_hi);
    }
    for (int i = 0; i < max_iter; ++i) {
        const double mid = 0.5 * (lo + hi);
        const double f_mid = f(mid);
        if (!std::isfinite(f_mid)) {
            hi = mid;
            continue;
        }
        if (std::abs(f_mid) <= pressure_tol_pa) {
            return mid;
        }
        if (f_mid <= 0.0) {
            lo = mid;
        } else {
            hi = mid;
        }
        if (std::abs(hi - lo) <= 1e-9) {
            return mid;
        }
    }
    return 0.5 * (lo + hi);
}

struct geometry_t {
    double pressure_in{};
    double volume_flow_q{};
    double length_main1_m{};
    double length_main2_m{};
    double length_lateral_m{};
    double d_magistral{};
    double d_branch1{};
    double wall{};
};

inline double compute_and_solve_d2(const geometry_t& g, const oil_properties_t& oil, double d_min, double d_max) {
    const double q = g.volume_flow_q;
    const double two_q = 2.0 * q;
    const double p1 = outlet_pressure_pipe_pq(g.pressure_in, two_q, g.length_main1_m, g.d_magistral, g.wall, oil);
    const double p_out1 =
        outlet_pressure_branch_pq(p1, q, g.d_magistral, g.d_branch1, g.length_lateral_m, g.wall, oil);
    const double p2 = outlet_pressure_pipe_pq(p1, q, g.length_main2_m, g.d_magistral, g.wall, oil);
    constexpr int k_iters = 80;
    constexpr double tol_pa = 0.5;
    return solve_d2_bisection(p2, p_out1, q, g.d_magistral, g.length_lateral_m, g.wall, oil, d_min, d_max, k_iters, tol_pa);
}

} // namespace hydraulics_struct::tz36_chain
