#pragma once

/// @brief Численные методы (биссекция, Ньютон), производная и диагностика итераций.

#include <cstddef>
#include <functional>
#include <limits>
#include <vector>

namespace numerical {

// =============================================================================
// Полиморфный интерфейс уравнения
// =============================================================================

/// @brief Тип численного метода для единой точки запуска.
enum class root_method_t {
    /// Метод биссекции.
    bisection,
    /// Метод Ньютона.
    newton
};

/// @brief Абстракция уравнения r(x)=0 для обобщенного расчетчика.
class root_equation_t {
public:
    virtual ~root_equation_t() = default;

    /// @brief Невязка r(x).
    virtual double residual(double x) const = 0;

    /// @brief Производная r'(x); по умолчанию вычисляется численно.
    virtual double derivative(double x) const;
};

// =============================================================================
// Критерии останова
// =============================================================================

/// @brief Причина остановки численного метода.
enum class stop_criterion_t {
    /// Остановка по аргументу.
    by_argument,
    /// Остановка по невязке.
    by_residual,
    /// Остановка по числу итераций.
    by_iterations
};

// =============================================================================
// Настройки расчёта
// =============================================================================

/// @brief Параметры запуска численного метода.
struct method_settings_t {
    /// Общий epsilon для критериев остановки.
    double epsilon{1e-8};
    /// Включить критерий по аргументу.
    bool use_epsilon_x{true};
    /// Включить критерий по невязке.
    bool use_epsilon_r{false};
    /// Максимальное количество итераций.
    std::size_t iteration_count{100};
    /// Включить сбор диагностики.
    bool collect_diagnostics{false};
    /// Подсказка для предварительного reserve истории.
    std::size_t diagnostics_reserve_hint{0};
};

// =============================================================================
// Диагностика расчёта
// =============================================================================

/// @brief Точка истории итераций.
struct iteration_trace_point_t {
    /// Номер итерации.
    std::size_t iteration{0};
    /// Текущее значение аргумента.
    double x{std::numeric_limits<double>::quiet_NaN()};
    /// Невязка на текущей итерации.
    double residual{std::numeric_limits<double>::quiet_NaN()};
    /// Шаг по аргументу.
    double delta_x{std::numeric_limits<double>::quiet_NaN()};
};

/// @brief Диагностика процесса сходимости.
struct diagnostics_t {
    /// История точек итераций.
    std::vector<iteration_trace_point_t> history{};
    /// Признак монотонной сходимости по |residual|.
    bool is_monotonic_convergence{false};
};

// =============================================================================
// Результаты расчёта
// =============================================================================

/// @brief Итог численного метода.
struct method_result_t {
    /// Найденный корень.
    double root{std::numeric_limits<double>::quiet_NaN()};
    /// Невязка в финальной точке.
    double residual{std::numeric_limits<double>::quiet_NaN()};
    /// Число выполненных итераций.
    std::size_t iterations{0};
    /// Сработавший критерий остановки.
    stop_criterion_t stop_criterion{stop_criterion_t::by_iterations};
    /// Диагностическая информация по итерациям.
    diagnostics_t diagnostics{};
};

// =============================================================================
// Численные методы
// =============================================================================

/// @brief Решение уравнения методом биссекции.
method_result_t solve_bisection(
    const std::function<double(double)>& r,
    double left,
    double right,
    const method_settings_t& settings = {}
);

/// @brief Решение уравнения методом Ньютона.
method_result_t solve_newton(
    const std::function<double(double)>& r,
    const std::function<double(double)>& dr,
    double x0,
    const method_settings_t& settings = {}
);

/// @brief Единая точка запуска поиска корня (черный ящик для пользователя).
method_result_t solve_root(
    const root_equation_t& equation,
    root_method_t method,
    const method_settings_t& settings,
    double left = std::numeric_limits<double>::quiet_NaN(),
    double right = std::numeric_limits<double>::quiet_NaN(),
    double x0 = std::numeric_limits<double>::quiet_NaN()
);

/// @brief Построение численной производной через центральную разность.
std::function<double(double)> make_numerical_derivative(
    const std::function<double(double)>& r,
    double h = 1e-8
);

} //namespace