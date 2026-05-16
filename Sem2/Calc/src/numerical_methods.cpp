#include "numerical_methods.h"

/// @brief Реализация численных методов и сервисных функций диагностики.

#include <cmath>
#include <string>
#include <stdexcept>

namespace numerical {

double root_equation_t::derivative(double x) const {
    const auto dr_num = make_numerical_derivative(
        [this](double value) { return residual(value); }
    );
    return dr_num(x);
}

namespace {

// =============================================================================
// Служебные константы и функции
// =============================================================================

/// @brief Допуск при проверке монотонности |residual|.
constexpr double monotonic_tolerance = 1e-12;

/// @brief Формирует контекстный префикс ошибки (метод/этап/итерация).
std::string build_prefix(const char* method_name, const char* stage, std::size_t iteration) {
    return "[" + std::string(method_name) + "] iteration " + std::to_string(iteration) + ", stage " + stage + ": ";
}

/// @brief Безопасный вызов пользовательской функции с проверкой finiteness.
double evaluate_function(
    const std::function<double(double)>& fn,
    double x,
    const char* method_name,
    const char* stage,
    std::size_t iteration
) {
    const std::string prefix = build_prefix(method_name, stage, iteration);
    try {
        const double value = fn(x);
        if (!std::isfinite(value)) {
            throw std::runtime_error(prefix + "функция вернула нечисловое значение");
        }
        return value;
    } catch (const std::runtime_error&) {
        throw;
    } catch (const std::exception& e) {
        throw std::runtime_error(prefix + "функция выбросила исключение: " + e.what());
    } catch (...) {
        throw std::runtime_error(prefix + "функция выбросила неизвестное исключение");
    }
}

/// @brief Инициализирует контейнер диагностики с возможным reserve.
diagnostics_t init_diagnostics(const method_settings_t& settings) {
    diagnostics_t diagnostics;
    if (settings.collect_diagnostics) {
        diagnostics.is_monotonic_convergence = true;
        if (settings.diagnostics_reserve_hint > 0) {
            diagnostics.history.reserve(settings.diagnostics_reserve_hint);
        }
    }
    return diagnostics;
}

/// @brief Добавляет точку в диагностику и обновляет флаг монотонности.
void append_diagnostics_point(
    diagnostics_t& diagnostics,
    const method_settings_t& settings,
    std::size_t iteration,
    double x,
    double residual,
    double delta_x
) {
    if (!settings.collect_diagnostics) {
        return;
    }

    if (!diagnostics.history.empty()) {
        const double prev_abs = std::abs(diagnostics.history.back().residual);
        const double curr_abs = std::abs(residual);
        if (curr_abs > prev_abs + monotonic_tolerance) {
            diagnostics.is_monotonic_convergence = false;
        }
    }

    diagnostics.history.push_back({iteration, x, residual, delta_x});
}

}

// =============================================================================
// Метод биссекции
// =============================================================================

method_result_t solve_bisection(
    const std::function<double(double)>& r,
    double left,
    double right,
    const method_settings_t& settings
) {
    diagnostics_t diagnostics = init_diagnostics(settings);

    if (!std::isfinite(left) || !std::isfinite(right) || settings.iteration_count == 0) {
        throw std::runtime_error("Некорректные параметры метода биссекции");
    }
    if (!settings.use_epsilon_x && !settings.use_epsilon_r) {
        throw std::runtime_error("Не выбран критерий остановки метода биссекции");
    }
    if ((!std::isfinite(settings.epsilon) || settings.epsilon <= 0.0)) {
        throw std::runtime_error("Некорректный общий epsilon метода биссекции");
    }
    if (settings.use_epsilon_x && (!std::isfinite(settings.epsilon) || settings.epsilon <= 0.0)) {
        throw std::runtime_error("Некорректный критерий по аргументу для метода биссекции");
    }
    if (settings.use_epsilon_r && (!std::isfinite(settings.epsilon) || settings.epsilon <= 0.0)) {
        throw std::runtime_error("Некорректный критерий по невязке для метода биссекции");
    }
    if (left > right) {
        throw std::runtime_error("Некорректные параметры: конечное значение отрезка меньше начального");
    }

    const double f_left = evaluate_function(r, left, "bisection", "left-boundary", 0);
    const double f_right = evaluate_function(r, right, "bisection", "right-boundary", 0);
    append_diagnostics_point(diagnostics, settings, 0, left, f_left, std::abs(right - left));
    if (f_left == 0.0) {
        return method_result_t{left, f_left, 0, stop_criterion_t::by_residual, diagnostics};
    }
    append_diagnostics_point(diagnostics, settings, 0, right, f_right, std::abs(right - left));
    if (f_right == 0.0) {
        return method_result_t{right, f_right, 0, stop_criterion_t::by_residual, diagnostics};
    }
    if (f_left * f_right > 0.0) {
        throw std::runtime_error("На интервале нет смены знака функции");
    }

    double x_left = left;
    double x_right = right;
    double f_x_left = f_left;
    double x_mid = std::numeric_limits<double>::quiet_NaN();
    double f_mid = std::numeric_limits<double>::quiet_NaN();

    for (std::size_t i = 0; i < settings.iteration_count; ++i) {
        x_mid = 0.5 * (x_left + x_right);
        f_mid = evaluate_function(r, x_mid, "bisection", "midpoint", i + 1);
        append_diagnostics_point(diagnostics, settings, i + 1, x_mid, f_mid, std::abs(x_right - x_left));

        if (f_mid == 0.0) {
            return method_result_t{x_mid, f_mid, i + 1, stop_criterion_t::by_residual, diagnostics};
        }

        if (settings.use_epsilon_r && std::abs(f_mid) <= settings.epsilon) {
            return method_result_t{x_mid, f_mid, i + 1, stop_criterion_t::by_residual, diagnostics};
        }
        if (settings.use_epsilon_x && (x_right - x_left) <= settings.epsilon) {
            return method_result_t{x_mid, f_mid, i + 1, stop_criterion_t::by_argument, diagnostics};
        }

        if ((f_x_left < 0.0 && f_mid > 0.0) || (f_x_left > 0.0 && f_mid < 0.0)) {
            x_right = x_mid;
        } else {
            x_left = x_mid;
            f_x_left = f_mid;
        }
    }

    return method_result_t{x_mid, f_mid, settings.iteration_count, stop_criterion_t::by_iterations, diagnostics};
}

// =============================================================================
// Метод Ньютона
// =============================================================================

method_result_t solve_newton(
    const std::function<double(double)>& r,
    const std::function<double(double)>& dr,
    double x0,
    const method_settings_t& settings
) {
    diagnostics_t diagnostics = init_diagnostics(settings);

    if (!std::isfinite(x0) || settings.iteration_count == 0) {
        throw std::runtime_error("Некорректные параметры метода Ньютона");
    }
    if (!settings.use_epsilon_x && !settings.use_epsilon_r) {
        throw std::runtime_error("Не выбран критерий остановки метода Ньютона");
    }
    if ((!std::isfinite(settings.epsilon) || settings.epsilon <= 0.0)) {
        throw std::runtime_error("Некорректный общий epsilon метода Ньютона");
    }
    if (settings.use_epsilon_x && (!std::isfinite(settings.epsilon) || settings.epsilon <= 0.0)) {
        throw std::runtime_error("Некорректный критерий по аргументу для метода Ньютона");
    }
    if (settings.use_epsilon_r && (!std::isfinite(settings.epsilon) || settings.epsilon <= 0.0)) {
        throw std::runtime_error("Некорректный критерий по невязке для метода Ньютона");
    }

    double x = x0;
    double prev_x = x0;
    for (std::size_t i = 0; i < settings.iteration_count; ++i) {
        const double fx = evaluate_function(r, x, "newton", "function-value", i + 1);
        const double delta_x = (i == 0) ? 0.0 : std::abs(x - prev_x);
        append_diagnostics_point(diagnostics, settings, i + 1, x, fx, delta_x);
        if (settings.use_epsilon_r && std::abs(fx) <= settings.epsilon) {
            return method_result_t{x, fx, i, stop_criterion_t::by_residual, diagnostics};
        }

        const double dfx = evaluate_function(dr, x, "newton", "derivative-value", i + 1);
        if (std::abs(dfx) < std::numeric_limits<double>::epsilon()) {
            throw std::runtime_error("Производная близка к нулю");
        }

        const double x_next = x - fx / dfx;
        if (!std::isfinite(x_next)) {
            throw std::runtime_error("Следующее приближение метода Ньютона не является числом");
        }
        if (settings.use_epsilon_x && std::abs(x_next - x) <= settings.epsilon) {
            const double fx_next = evaluate_function(r, x_next, "newton", "function-value-next", i + 1);
            append_diagnostics_point(diagnostics, settings, i + 1, x_next, fx_next, std::abs(x_next - x));
            return method_result_t{x_next, fx_next, i + 1, stop_criterion_t::by_argument, diagnostics};
        }
        prev_x = x;
        x = x_next;
    }

    const double fx = evaluate_function(r, x, "newton", "final-value", settings.iteration_count);
    append_diagnostics_point(diagnostics, settings, settings.iteration_count, x, fx, std::abs(x - prev_x));
    return method_result_t{x, fx, settings.iteration_count, stop_criterion_t::by_iterations, diagnostics};
}

method_result_t solve_root(
    const root_equation_t& equation,
    root_method_t method,
    const method_settings_t& settings,
    double left,
    double right,
    double x0
) {
    if (method == root_method_t::bisection) {
        if (!std::isfinite(left) || !std::isfinite(right)) {
            throw std::runtime_error("Для метода биссекции должны быть заданы left/right");
        }
        return solve_bisection(
            [&equation](double x) { return equation.residual(x); },
            left,
            right,
            settings
        );
    }

    if (method == root_method_t::newton) {
        if (!std::isfinite(x0)) {
            throw std::runtime_error("Для метода Ньютона должно быть задано x0");
        }
        return solve_newton(
            [&equation](double x) { return equation.residual(x); },
            [&equation](double x) { return equation.derivative(x); },
            x0,
            settings
        );
    }

    throw std::runtime_error("Неизвестный тип численного метода");
}

// =============================================================================
// Численная производная
// =============================================================================

std::function<double(double)> make_numerical_derivative(
    const std::function<double(double)>& r,
    double h
) {
    if (!std::isfinite(h) || h <= 0.0) {
        throw std::runtime_error("Некорректный шаг численной производной");
    }

    return [r, h](double x) {
        const double local_h = h * std::max(1.0, std::abs(x));
        const double f_plus = evaluate_function(r, x + local_h, "derivative", "plus", 0);
        const double f_minus = evaluate_function(r, x - local_h, "derivative", "minus", 0);
        const double derivative = (f_plus - f_minus) / (2.0 * local_h);
        if (!std::isfinite(derivative)) {
            throw std::runtime_error("[derivative] stage result: производная не является числом");
        }
        return derivative;
    };
}

} //namespace