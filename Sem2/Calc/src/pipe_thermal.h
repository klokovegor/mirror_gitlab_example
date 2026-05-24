#pragma once

/// @brief TZ46: прогрев трубы с неподвижным теплоносителем (RC-модель, FTCS по оси).

#include <cstddef>
#include <limits>
#include <memory>
#include <string>

namespace thermal_struct {

/// @brief Ускорение свободного падения g, м/с² (для числа Грасгофа).
constexpr double gravity_acceleration = 9.81;

// =============================================================================
// Свойства и геометрия
// =============================================================================

/// @brief Свойства теплоносителя (вода и др.).
struct fluid_properties_t {
    /// @brief Плотность ρ_f, кг/м³.
    double density{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Удельная теплоёмкость c_{p,f}, Дж/(кг·К).
    double specific_heat{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Теплопроводность λ_f, Вт/(м·К).
    double thermal_conductivity{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Кинематическая вязкость ν, м²/с (для Gr, Pr).
    double kinematic_viscosity{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Коэффициент объёмного расширения β, 1/К (для Gr).
    double volumetric_expansion{std::numeric_limits<double>::quiet_NaN()};

    void check_parameters() const;
};

/// @brief Свойства материала стенки трубы.
struct wall_material_properties_t {
    /// @brief Плотность стенки ρ_w, кг/м³.
    double density{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Удельная теплоёмкость стенки c_{p,w}, Дж/(кг·К).
    double specific_heat{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Теплопроводность стенки λ_wall, Вт/(м·К).
    double thermal_conductivity{std::numeric_limits<double>::quiet_NaN()};

    void check_parameters() const;
};

/// @brief Геометрия трубы для теплового расчёта.
struct pipe_thermal_geometry_t {
    /// @brief Внутренний диаметр d_in, м.
    double inner_diameter{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Наружный диаметр d_out, м.
    double outer_diameter{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Длина участка L, м.
    double length{std::numeric_limits<double>::quiet_NaN()};

    static pipe_thermal_geometry_t create(double inner_diameter_val, double outer_diameter_val,
                                          double length_val);

    /// @brief Толщина стенки δ = (d_out − d_in) / 2, м.
    double wall_thickness() const;

    void check_parameters() const;
};

/// @brief Температурные границы и внешний теплоотдача.
struct thermal_boundary_t {
    /// @brief Температура подачи на входе T_supply, °C (граничное условие x = 0).
    double supply_temperature{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Начальная температура теплоносителя T_initial, °C (t = 0).
    double initial_temperature{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Температура окружающей среды T_amb, °C.
    double ambient_temperature{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Коэффициент теплоотдачи наружу α_ext, Вт/(м²·К).
    double external_htc{std::numeric_limits<double>::quiet_NaN()};

    void check_parameters() const;
};

// =============================================================================
// Результаты
// =============================================================================

/// @brief Тепловые сопротивления на 1 м длины трубы (последовательное соединение слоёв).
struct thermal_resistance_result_t {
    /// @brief Сопротивление теплоотдаче внутри R_int = 1/(α_int·π·d_in), (К·м)/Вт.
    double r_int{0.0};
    /// @brief Сопротивление стенки R_wall = ln(d_out/d_in)/(2π·λ_wall), (К·м)/Вт.
    double r_wall{0.0};
    /// @brief Сопротивление теплоотдаче наружу R_ext = 1/(α_ext·π·d_out), (К·м)/Вт.
    double r_ext{0.0};
    /// @brief Суммарное сопротивление R_Σ = R_int + R_wall + R_ext, (К·м)/Вт.
    double r_total{0.0};
    /// @brief Использованный коэффициент теплоотдачи внутри α_int, Вт/(м²·К).
    double alpha_int{0.0};
    /// @brief Критерий Био Bi = α_ext·δ/λ_wall (проверка сосредоточенной ёмкости).
    double biot{0.0};
};

/// @brief Тепловая ёмкость участка.
struct thermal_capacity_result_t {
    /// @brief Теплоёмкость на 1 м C_th, Дж/(К·м) (жидкость + стенка).
    double capacity_per_meter{0.0};
};

/// @brief Результат переходного процесса (экспоненциальная модель).
struct thermal_transient_result_t {
    /// @brief Постоянная времени τ = R_Σ·C_th, с.
    double time_constant{0.0};
    /// @brief Время достижения доли η: t_η = −τ·ln(1−η), с.
    double time_to_fraction{0.0};
    /// @brief Заданная доля прогрева η от (T_supply − T_amb).
    double fraction{0.0};
};

/// @brief Сравнение прогрева без конвекции и с естественной конвекцией.
struct convection_comparison_t {
    /// @brief Режим чистой теплопроводности (α_int ≈ λ_f/d_in).
    thermal_transient_result_t conduction;
    /// @brief Режим с естественной конвекцией (Nu = C·(Gr·Pr)^n).
    thermal_transient_result_t convection;
    /// @brief Отношение τ_conv / τ_cond (< 1 — конвекция ускоряет прогрев).
    double tau_ratio{0.0};
};

/// @brief Рекомендация по предварительному циркуляционному прогреву.
enum class circulation_recommendation_t {
    recommended, ///< Циркуляция целесообразна (длинная труба, большой ΔT или долгий прогрев).
    optional,    ///< Допустимо с циркуляцией или без неё.
    not_needed   ///< Короткий участок, прогрев без насоса допустим.
};

/// @brief Текстовый вывод по циркуляционному прогреву.
struct circulation_advice_result_t {
    circulation_recommendation_t recommendation{circulation_recommendation_t::optional};
    std::string message;
};

// =============================================================================
// Полиморфные модели внутреннего теплоотдачи
// =============================================================================

/// @brief Модель коэффициента теплоотдачи внутри трубы α_int.
class internal_htc_model_t {
public:
    virtual ~internal_htc_model_t() = default;

    /// @param delta_t_char характерный перепад температур для Gr, К.
    virtual double alpha_int(double inner_diameter, const fluid_properties_t& fluid,
                             double delta_t_char) const = 0;
};

/// @brief Неподвижная жидкость: α_int ≈ λ_f / d_in (task.md).
class stagnant_conduction_htc_t : public internal_htc_model_t {
public:
    double alpha_int(double inner_diameter, const fluid_properties_t& fluid,
                     double delta_t_char) const override;
};

/// @brief Естественная конвекция: Nu = C·(Gr·Pr)^n, α_int = Nu·λ_f/d_in.
class natural_convection_htc_t : public internal_htc_model_t {
public:
    explicit natural_convection_htc_t(double correlation_c, double correlation_n);

    double alpha_int(double inner_diameter, const fluid_properties_t& fluid,
                     double delta_t_char) const override;

private:
    /// @brief Коэффициент C в корреляции Nu = C·(Gr·Pr)^n (задаёт пользователь).
    double correlation_c_{0.0};
    /// @brief Показатель n в корреляции Nu = C·(Gr·Pr)^n (задаёт пользователь).
    double correlation_n_{0.0};
};

/// @brief Заданный пользователем α_int, Вт/(м²·К).
class fixed_alpha_htc_t : public internal_htc_model_t {
public:
    explicit fixed_alpha_htc_t(double alpha);

    double alpha_int(double inner_diameter, const fluid_properties_t& fluid,
                     double delta_t_char) const override;

private:
    double alpha_{0.0};
};

// =============================================================================
// RC-калькулятор
// =============================================================================

/// @brief Расчёт R_Σ, C_th, τ и сравнение режимов по RC-модели (на 1 м).
class pipe_thermal_calculator_t {
public:
    pipe_thermal_calculator_t(pipe_thermal_geometry_t geometry, thermal_boundary_t boundary,
                              fluid_properties_t fluid, wall_material_properties_t wall,
                              std::unique_ptr<internal_htc_model_t> htc_model);

    thermal_resistance_result_t compute_resistances_per_meter() const;

    thermal_capacity_result_t compute_capacity_per_meter() const;

    /// @brief Постоянная времени τ = R_Σ·C_th, с.
    double compute_time_constant() const;

    /// @param eta доля прогрева (0 < η < 1), например 0.95.
    thermal_transient_result_t time_to_fraction(double eta) const;

    /// @param nu_correlation_c коэффициент C в Nu = C·(Gr·Pr)^n.
    /// @param nu_correlation_n показатель n в Nu = C·(Gr·Pr)^n.
    /// @param delta_t_char перепад для Gr, К.
    /// @param fluid_for_convection свойства теплоносителя для ветки с Nu (часто при ~50 °C).
    convection_comparison_t compare_conduction_vs_convection(
        double nu_correlation_c, double nu_correlation_n, double delta_t_char, double eta,
        const fluid_properties_t& fluid_for_convection) const;

    circulation_advice_result_t circulation_advice() const;

    const pipe_thermal_geometry_t& geometry() const { return geometry_; }
    const thermal_boundary_t& boundary() const { return boundary_; }
    const fluid_properties_t& fluid() const { return fluid_; }

private:
    thermal_resistance_result_t resistances_with_model(
        const internal_htc_model_t& model, double delta_t_char,
        const fluid_properties_t& fluid) const;

    pipe_thermal_geometry_t geometry_;
    thermal_boundary_t boundary_;
    fluid_properties_t fluid_;
    wall_material_properties_t wall_;
    std::unique_ptr<internal_htc_model_t> htc_model_;
};

// =============================================================================
// FTCS по оси (task.md)
// =============================================================================

/// @brief Критерий остановки явной схемы по температуре.
enum class ftcs_stop_criterion_t {
    mean_temperature,   ///< Средняя температура по сечениям трубы.
    outlet_temperature  ///< Температура на конце x = L.
};

/// @brief Параметры явной разностной схемы FTCS вдоль оси x.
struct ftcs_settings_t {
    /// @brief Число узлов сетки по длине (включая границы).
    std::size_t nodes{101};
    /// @brief Шаг по времени Δt, с.
    double time_step{std::numeric_limits<double>::quiet_NaN()};
    /// @brief Максимальное время интегрирования, с.
    double max_time{std::numeric_limits<double>::quiet_NaN()};
    ftcs_stop_criterion_t stop{ftcs_stop_criterion_t::mean_temperature};
    /// @brief Целевая доля прогрева от (T_supply − T_amb), например 0.95.
    double target_fraction{0.95};
};

/// @brief Результат интегрирования FTCS.
struct ftcs_result_t {
    /// @brief Накопленное физическое время, с.
    double elapsed_time{0.0};
    /// @brief Итоговая температура (средняя или на выходе — по stop).
    double final_temperature{0.0};
    /// @brief Достигнута ли target_fraction до max_time.
    bool reached_target{false};
    /// @brief Число выполненных шагов по времени.
    std::size_t steps{0};
};

/// @brief Численный прогрев вдоль длинной трубы (явная схема FTCS).
class axial_conduction_solver_t {
public:
    /// @param u_overall_per_meter коэффициент теплопередачи U = 1/R_Σ, Вт/(м²·К) на 1 м.
    axial_conduction_solver_t(pipe_thermal_geometry_t geometry, thermal_boundary_t boundary,
                              fluid_properties_t fluid, double u_overall_per_meter,
                              ftcs_settings_t settings);

    ftcs_result_t run_until_fraction() const;

private:
    pipe_thermal_geometry_t geometry_;
    thermal_boundary_t boundary_;
    fluid_properties_t fluid_;
    /// @brief U для члена потерь через стенку, Вт/(м²·К) на 1 м длины.
    double u_overall_per_meter_{0.0};
    ftcs_settings_t settings_;
};

} // namespace thermal_struct
