/// @brief Тесты метода биссекции и метода Ньютона на базовых типах уравнений.

#include "numerical_methods.h"

#include "gtest/gtest.h"

#include <fixed/fixed.h>
#include <fixed/fixed_bisection.h>

#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <string>
#include <vector>


using namespace numerical;

/// @brief Проверяет биссекцию для линейного уравнения x - 5 = 0.
TEST(NumericalMethodsBisection, SolvesLinearEquation) {
    // Arrange
    const auto f = [](double x) { return x - 5.0; };
    const method_settings_t settings{1e-8, true, true, 100};
    // Act
    const method_result_t result = solve_bisection(f, 0.0, 10.0, settings);
    // Assert
    EXPECT_NEAR(result.root, 5.0, 1e-8);
}

/// @brief Проверяет биссекцию для квадратного уравнения x^2 - 6x + 8 = 0.
TEST(NumericalMethodsBisection, SolvesQuadraticEquation) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    const method_settings_t settings{1e-8, true, true, 100};
    // Act
    const method_result_t result = solve_bisection(f, 1.0, 3.0, settings);
    // Assert
    EXPECT_NEAR(result.root, 2.0, 1e-8);
}

/// @brief Проверяет биссекцию для гиперболического уравнения 1/x - 2 = 0.
TEST(NumericalMethodsBisection, SolvesHyperbolicEquation) {
    // Arrange
    const auto f = [](double x) { return 1.0 / x - 2.0; };
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_bisection(f, 0.1, 1.0, settings);
    // Assert
    EXPECT_NEAR(result.root, 0.5, 1e-8);
}

/// @brief Проверяет биссекцию для гармонической функции sin(x) = 0.
TEST(NumericalMethodsBisection, SolvesHarmonicEquation) {
    // Arrange
    const auto f = [](double x) { return std::sin(x); };
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_bisection(f, 3.0, 4.0, settings);
    // Assert
    EXPECT_NEAR(result.root, std::numbers::pi, 1e-8);
}

/// @brief Проверяет метод Ньютона для линейного уравнения x - 5 = 0.
TEST(NumericalMethodsNewton, SolvesLinearEquation) {
    // Arrange
    const auto f = [](double x) { return x - 5.0; };
    const auto df = [](double /*x*/) { return 1.0; };
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_newton(f, df, 0.0, settings);
    // Assert
    EXPECT_NEAR(result.root, 5.0, 1e-8);
}

/// @brief Проверяет метод Ньютона для квадратного уравнения x^2 - 6x + 8 = 0.
TEST(NumericalMethodsNewton, SolvesQuadraticEquation) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    const auto df = [](double x) { return 2.0 * x - 6.0; };
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_newton(f, df, 1.5, settings);
    // Assert
    EXPECT_NEAR(result.root, 2.0, 1e-8);
}

/// @brief Проверяет метод Ньютона для гиперболического уравнения 1/x - 2 = 0.
TEST(NumericalMethodsNewton, SolvesHyperbolicEquation) {
    // Arrange
    const auto f = [](double x) { return 1.0 / x - 2.0; };
    const auto df = [](double x) { return -1.0 / (x * x); };
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_newton(f, df, 0.25, settings);
    // Assert
    EXPECT_NEAR(result.root, 0.5, 1e-8);
}

/// @brief Проверяет метод Ньютона для гармонической функции sin(x) = 0.
TEST(NumericalMethodsNewton, SolvesHarmonicEquation) {
    // Arrange
    const auto f = [](double x) { return std::sin(x); };
    const auto df = [](double x) { return std::cos(x); };
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_newton(f, df, 3.0, settings);
    // Assert
    EXPECT_NEAR(result.root, std::numbers::pi, 1e-8);
}

/// @brief Проверяет остановку биссекции по ширине интервала аргумента.
TEST(NumericalMethodsBisection, StopsByArgumentCriterion) {
    // Arrange
    const auto f = [](double x) { return x * x * x - x - 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_bisection(f, 1.0, 2.0, settings);
    // Assert
    EXPECT_EQ(result.stop_criterion, stop_criterion_t::by_argument);
}

/// @brief Проверяет остановку биссекции по невязке.
TEST(NumericalMethodsBisection, StopsByResidualCriterion) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = false;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_bisection(f, 1.0, 3.0, settings);
    // Assert
    EXPECT_EQ(result.stop_criterion, stop_criterion_t::by_residual);
}

/// @brief Проверяет остановку биссекции по лимиту числа итераций.
TEST(NumericalMethodsBisection, StopsByIterationsCriterion) {
    // Arrange
    const auto f = [](double x) { return x - 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_bisection(f, 0.0, 1e30, settings);
    // Assert
    EXPECT_EQ(result.stop_criterion, stop_criterion_t::by_iterations);
}

/// @brief Проверяет остановку Ньютона по приращению аргумента.
TEST(NumericalMethodsNewton, StopsByArgumentCriterion) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    const auto df = [](double x) { return 2.0 * x - 6.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_newton(f, df, 1.7, settings);
    // Assert
    EXPECT_EQ(result.stop_criterion, stop_criterion_t::by_argument);
}

/// @brief Проверяет остановку Ньютона по невязке.
TEST(NumericalMethodsNewton, StopsByResidualCriterion) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    const auto df = [](double x) { return 2.0 * x - 6.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = false;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_newton(f, df, 1.7, settings);
    // Assert
    EXPECT_EQ(result.stop_criterion, stop_criterion_t::by_residual);
}

/// @brief Проверяет остановку Ньютона по лимиту числа итераций.
TEST(NumericalMethodsNewton, StopsByIterationsCriterion) {
    // Arrange
    const auto f = [](double x) { return std::pow(x - 1.0, 10); };
    const auto df = [](double x) { return 10.0 * std::pow(x - 1.0, 9); };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_newton(f, df, 10000.0, settings);
    // Assert
    EXPECT_EQ(result.stop_criterion, stop_criterion_t::by_iterations);
}

/// @brief Проверяет, что биссекция завершает расчет при NaN из функции невязки.
TEST(NumericalMethodsBisection, ThrowsIfResidualReturnsNaN) {
    // Arrange
    const auto f = [](double x) {
        if (x > 0.49 && x < 0.51) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return x - 0.5;
    };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act & Assert
    EXPECT_THROW(static_cast<void>(solve_bisection(f, 0.0, 1.0, settings)), std::runtime_error);
}

/// @brief Проверяет, что биссекция оборачивает исключение из функции невязки в runtime_error.
TEST(NumericalMethodsBisection, ThrowsIfResidualFunctionThrows) {
    // Arrange
    const auto f = [](double x) {
        if (x > 0.49 && x < 0.51) {
            throw std::runtime_error("user residual error");
        }
        return x - 0.5;
    };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act & Assert
    EXPECT_THROW(static_cast<void>(solve_bisection(f, 0.0, 1.0, settings)), std::runtime_error);
}

/// @brief Проверяет, что Ньютон завершает расчет при NaN из функции невязки.
TEST(NumericalMethodsNewton, ThrowsIfResidualReturnsNaN) {
    // Arrange
    const auto f = [](double x) {
        if (x > 1.9) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return x - 1.0;
    };
    const auto df = [](double /*x*/) { return 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act & Assert
    EXPECT_THROW(static_cast<void>(solve_newton(f, df, 2.0, settings)), std::runtime_error);
}

/// @brief Проверяет, что Ньютон завершает расчет при NaN из производной.
TEST(NumericalMethodsNewton, ThrowsIfDerivativeReturnsNaN) {
    // Arrange
    const auto f = [](double x) { return x - 1.0; };
    const auto df = [](double x) {
        if (x > 1.9) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return 1.0;
    };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act & Assert
    EXPECT_THROW(static_cast<void>(solve_newton(f, df, 2.0, settings)), std::runtime_error);
}

/// @brief Проверяет, что Ньютон оборачивает исключение из функции невязки в runtime_error.
TEST(NumericalMethodsNewton, ThrowsIfResidualFunctionThrows) {
    // Arrange
    const auto f = [](double x) {
        if (x > 1.9) {
            throw std::runtime_error("user residual error");
        }
        return x - 1.0;
    };
    const auto df = [](double /*x*/) { return 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act & Assert
    EXPECT_THROW(static_cast<void>(solve_newton(f, df, 2.0, settings)), std::runtime_error);
}

/// @brief Проверяет, что Ньютон оборачивает исключение из производной в runtime_error.
TEST(NumericalMethodsNewton, ThrowsIfDerivativeFunctionThrows) {
    // Arrange
    const auto f = [](double x) { return x - 1.0; };
    const auto df = [](double x) {
        if (x > 1.9) {
            throw std::runtime_error("user derivative error");
        }
        return 1.0;
    };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    // Act & Assert
    EXPECT_THROW(static_cast<void>(solve_newton(f, df, 2.0, settings)), std::runtime_error);
}

/// @brief Проверяет точность численной производной в контрольной точке.
TEST(NumericalMethodsDerivative, NumericalDerivativeMatchesAnalyticalValue) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    const auto dr_num = make_numerical_derivative(f, 1e-6);
    // Act
    const double value = dr_num(2.3);
    // Assert
    EXPECT_NEAR(value, -1.4, 1e-5);
}

/// @brief Проверяет Ньютон с пользовательской численной производной.
TEST(NumericalMethodsNewton, SolvesQuadraticEquationWithUserNumericalDerivative) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    const auto dr_num = make_numerical_derivative(f, 1e-6);
    const method_settings_t settings{1e-8, true, false, 100};
    // Act
    const method_result_t result = solve_newton(f, dr_num, 1.5, settings);
    // Assert
    EXPECT_NEAR(result.root, 2.0, 1e-6);
}

/// @brief Проверяет ошибку при невалидном шаге численной производной.
TEST(NumericalMethodsDerivative, ThrowsOnNonPositiveStep) {
    // Arrange
    const auto f = [](double x) { return x * x - 6.0 * x + 8.0; };
    // Act & Assert
    EXPECT_THROW(static_cast<void>(make_numerical_derivative(f, 0.0)), std::runtime_error);
    EXPECT_THROW(static_cast<void>(make_numerical_derivative(f, -1e-6)), std::runtime_error);
}

/// @brief Проверяет, что при выключенной диагностике история итераций не накапливается.
TEST(NumericalMethodsDiagnostics, DisabledDiagnosticsKeepsHistoryEmpty) {
    // Arrange
    const auto f = [](double x) { return x - 5.0; };
    const auto df = [](double /*x*/) { return 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 100;
    settings.collect_diagnostics = false;
    // Act
    const method_result_t result = solve_newton(f, df, 0.0, settings);
    // Assert
    EXPECT_TRUE(result.diagnostics.history.empty());
}

/// @brief Проверяет заполнение истории и базовых полей диагностики при включенном сборе.
TEST(NumericalMethodsDiagnostics, EnabledDiagnosticsCollectsHistoryPoints) {
    // Arrange
    const auto f = [](double x) { return x * x * x - x - 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 100;
    settings.collect_diagnostics = true;
    settings.diagnostics_reserve_hint = 64;
    // Act
    const method_result_t result = solve_bisection(f, 1.0, 2.0, settings);
    // Assert
    ASSERT_FALSE(result.diagnostics.history.empty());
    EXPECT_LE(result.diagnostics.history.size(), settings.iteration_count + 2);
    const auto& first = result.diagnostics.history.front();
    EXPECT_TRUE(std::isfinite(first.x));
    EXPECT_TRUE(std::isfinite(first.residual));
    EXPECT_TRUE(std::isfinite(first.delta_x));
}

/// @brief Проверяет признак монотонной сходимости для корректно заданного Ньютона.
TEST(NumericalMethodsDiagnostics, MonotonicConvergenceTrueForLinearNewton) {
    // Arrange
    const auto f = [](double x) { return x - 5.0; };
    const auto df = [](double /*x*/) { return 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = false;
    settings.use_epsilon_r = true;
    settings.iteration_count = 100;
    settings.collect_diagnostics = true;
    // Act
    const method_result_t result = solve_newton(f, df, 0.0, settings);
    // Assert
    EXPECT_TRUE(result.diagnostics.is_monotonic_convergence);
}

/// @brief Проверяет признак немонотонной сходимости для пользовательской производной.
TEST(NumericalMethodsDiagnostics, MonotonicConvergenceFalseForBadDerivative) {
    // Arrange
    const auto f = [](double x) { return std::sin(x); };
    const auto bad_df = [](double /*x*/) { return 1.0; };
    method_settings_t settings{};
    settings.epsilon = 1e-30;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 6;
    settings.collect_diagnostics = true;
    // Act
    const method_result_t result = solve_newton(f, bad_df, 3.0, settings);
    // Assert
    EXPECT_FALSE(result.diagnostics.is_monotonic_convergence);
}

namespace {

/// @brief Линейное уравнение r(x)=x-5 с аналитической производной.
class linear_equation_t : public root_equation_t {
public:
    double residual(double x) const override {
        return x - 5.0;
    }

    double derivative(double /*x*/) const override {
        return 1.0;
    }
};

/// @brief Гармоническое уравнение r(x)=sin(x), производная по умолчанию (численная).
class harmonic_equation_t : public root_equation_t {
public:
    double residual(double x) const override {
        return std::sin(x);
    }
};

}

/// @brief Проверяет единый полиморфный запуск для биссекции.
TEST(NumericalMethodsPolymorphism, SolveRootBisectionForLinearEquation) {
    // Arrange
    const linear_equation_t equation;
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_root(equation, root_method_t::bisection, settings, 0.0, 10.0);
    // Assert
    EXPECT_NEAR(result.root, 5.0, 1e-8);
}

/// @brief Проверяет единый полиморфный запуск для Ньютона с численной производной по умолчанию.
TEST(NumericalMethodsPolymorphism, SolveRootNewtonUsesDefaultNumericalDerivative) {
    // Arrange
    const harmonic_equation_t equation;
    method_settings_t settings{};
    settings.epsilon = 1e-8;
    settings.use_epsilon_x = true;
    settings.use_epsilon_r = false;
    settings.iteration_count = 100;
    // Act
    const method_result_t result = solve_root(equation, root_method_t::newton, settings, std::numeric_limits<double>::quiet_NaN(),
                                              std::numeric_limits<double>::quiet_NaN(), 3.0);
    // Assert
    EXPECT_NEAR(result.root, std::numbers::pi, 1e-6);
}

namespace {

struct validation_case_t {
    std::string name;
    std::function<double(double)> residual;
    std::function<double(double)> derivative;
    double left;
    double right;
    double x0;
};

double solve_fixed_bisection_reference(
    const std::function<double(double)>& residual,
    double left,
    double right,
    const method_settings_t& settings
) {
    fixed_scalar_wrapper_t equation(
        [&residual](double x) { return residual(x); },
        settings.epsilon
    );
    fixed_bisectional_parameters_t parameters;
    parameters.argument_limit_min = left;
    parameters.argument_limit_max = right;
    parameters.argument_precision = settings.epsilon;
    parameters.residual_precision = settings.epsilon;
    parameters.solution_type = fixed_bisectional_solution_type::Bisection;
    parameters.check_boundary_before = true;

    fixed_bisection_result_t<1> result;
    fixed_bisectional<1>::solve(parameters, equation, &result, nullptr);
    EXPECT_EQ(result.result_code, numerical_result_code_t::Converged);
    return result.argument;
}

double solve_fixed_newton_reference(
    const std::function<double(double)>& residual,
    double x0,
    const method_settings_t& settings
) {
    fixed_scalar_wrapper_t equation(
        [&residual](double x) { return residual(x); },
        settings.epsilon
    );
    fixed_solver_parameters_t<1> parameters;
    parameters.iteration_count = settings.iteration_count;
    parameters.argument_increment_norm = settings.epsilon;
    parameters.residuals_norm = settings.epsilon;
    parameters.residuals_norm_allow_early_exit = true;

    fixed_solver_result_t<1> result;
    fixed_newton_raphson<1>::solve(equation, x0, parameters, &result, nullptr);
    EXPECT_EQ(result.result_code, numerical_result_code_t::Converged);
    return result.argument;
}

}

/// @brief Проверяет численное совпадение корней биссекции с fixed_solvers.
TEST(NumericalMethodsValidation, BisectionMatchesFixedSolvers) {
    // Arrange
    const method_settings_t settings{1e-8, true, true, 100};
    const std::vector<validation_case_t> cases{
        {
            "linear",
            [](double x) { return x - 5.0; },
            [](double /*x*/) { return 1.0; },
            0.0,
            10.0,
            0.0
        },
        {
            "quadratic",
            [](double x) { return x * x - 6.0 * x + 8.0; },
            [](double x) { return 2.0 * x - 6.0; },
            1.0,
            3.0,
            1.5
        },
        {
            "hyperbolic",
            [](double x) { return 1.0 / x - 2.0; },
            [](double x) { return -1.0 / (x * x); },
            0.1,
            1.0,
            0.25
        },
        {
            "harmonic",
            [](double x) { return std::sin(x); },
            [](double x) { return std::cos(x); },
            3.0,
            4.0,
            3.0
        }
    };
    constexpr double tolerance = 1e-6;

    for (const auto& test_case : cases) {
        // Act
        const method_result_t our_result = solve_bisection(
            test_case.residual,
            test_case.left,
            test_case.right,
            settings
        );
        const double reference_root = solve_fixed_bisection_reference(
            test_case.residual,
            test_case.left,
            test_case.right,
            settings
        );

        // Assert
        EXPECT_NEAR(our_result.root, reference_root, tolerance)
            << "case=" << test_case.name
            << ", method=bisection"
            << ", our_root=" << our_result.root
            << ", reference_root=" << reference_root
            << ", abs_error=" << std::abs(our_result.root - reference_root);
    }
}

/// @brief Проверяет численное совпадение корней Ньютона с fixed_solvers.
TEST(NumericalMethodsValidation, NewtonMatchesFixedSolvers) {
    // Arrange
    const method_settings_t settings{1e-8, true, true, 100};
    const std::vector<validation_case_t> cases{
        {
            "linear",
            [](double x) { return x - 5.0; },
            [](double /*x*/) { return 1.0; },
            0.0,
            10.0,
            0.0
        },
        {
            "quadratic",
            [](double x) { return x * x - 6.0 * x + 8.0; },
            [](double x) { return 2.0 * x - 6.0; },
            1.0,
            3.0,
            1.5
        },
        {
            "hyperbolic",
            [](double x) { return 1.0 / x - 2.0; },
            [](double x) { return -1.0 / (x * x); },
            0.1,
            1.0,
            0.25
        },
        {
            "harmonic",
            [](double x) { return std::sin(x); },
            [](double x) { return std::cos(x); },
            3.0,
            4.0,
            3.0
        }
    };
    constexpr double tolerance = 1e-5;

    for (const auto& test_case : cases) {
        // Act
        const method_result_t our_result = solve_newton(
            test_case.residual,
            test_case.derivative,
            test_case.x0,
            settings
        );
        const double reference_root = solve_fixed_newton_reference(
            test_case.residual,
            test_case.x0,
            settings
        );

        // Assert
        EXPECT_NEAR(our_result.root, reference_root, tolerance)
            << "case=" << test_case.name
            << ", method=newton"
            << ", our_root=" << our_result.root
            << ", reference_root=" << reference_root
            << ", abs_error=" << std::abs(our_result.root - reference_root);
    }
}
