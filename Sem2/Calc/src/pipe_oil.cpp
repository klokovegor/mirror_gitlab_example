/// @brief Реализация переводов единиц, геометрии трубы, профиля давления и записи CSV.

#include "pipe_oil.h"

namespace hydraulics_struct {

namespace {

/// @brief Путь вида «буква:\...» в стиле Windows.
bool is_windows_absolute_path(const std::string& filepath) {

    return filepath.size() >= 3 &&
           std::isalpha(static_cast<unsigned char>(filepath[0])) &&
           filepath[1] == ':' &&
           (filepath[2] == '\\' || filepath[2] == '/');
}

/// @brief Допустим ли путь для безопасной записи CSV (родитель существует и т.п.).
bool is_writable_csv_path(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }

#ifndef _WIN32
    // На POSIX Windows-абсолютные пути невалидны и должны отклоняться.
    if (is_windows_absolute_path(filepath)) {
        return false;
    }
#endif

    const std::filesystem::path path(filepath);
    const auto parent = path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent)) {
        return false;
    }
    return true;
}

}

// ============================================================================
// Функции для вычисления единиц СИ
// ============================================================================

double length_si(double length, const std::string& unit) {
    std::map<std::string, std::function<double(double)>> converters = {
        {"км", [](double x) { return x * 1000; }},
        {"м", [](double x) { return x; }},
        {"см", [](double x) { return x / 100; }},
        {"мм", [](double x) { return x / 1000; }}
    };
    auto it = converters.find(unit);
    if (it != converters.end()) {
        return it->second(length);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double diameter_si(double diameter, const std::string& unit) {
    std::map<std::string, std::function<double(double)>> converters = {
        {"м", [](double x) { return x; }},
        {"см", [](double x) { return x / 100; }},
        {"мм", [](double x) { return x / 1000; }}
    };
    auto it = converters.find(unit);
    if (it != converters.end()) {
        return it->second(diameter);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double pressure_si(double pressure_value, const std::string& unit) {
    std::map<std::string, std::function<double(double)>> converters = {
        {"Па", [](double x) { return x; }},
        {"кПа", [](double x) { return x * 1e3; }},
        {"кгс/см^2", [](double x) { return x * 98066.5; }}
    };
    auto it = converters.find(unit);
    if (it != converters.end()) {
        return it->second(pressure_value);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double kinematic_viscosity_si(double kinematic_viscosity, const std::string& unit) {
    std::map<std::string, std::function<double(double)>> converters = {
        {"м^2/с", [](double x) { return x; }},
        {"сСт", [](double x) { return x * 1e-6; }}
    };
    auto it = converters.find(unit);
    if (it != converters.end()) {
        return it->second(kinematic_viscosity);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double dynamic_viscosity_si(double dynamic_viscosity, const std::string& unit) {
    std::map<std::string, std::function<double(double)>> converters = {
        {"Па·с", [](double x) { return x; }},
        {"сП", [](double x) { return x * 1e-2; }}
    };
    auto it = converters.find(unit);
    if (it != converters.end()) {
        return it->second(dynamic_viscosity);
    }
    return std::numeric_limits<double>::quiet_NaN();
}

// ============================================================================
// pipe_profile_t
// ============================================================================

pipe_profile_t pipe_profile_t::create_profile(std::vector<double> x, std::vector<double> z) {
    pipe_profile_t profile;
    profile.coordinates = x;
    profile.elevations = z;
    return profile;
}

size_t pipe_profile_t::get_point_count() const {
    return coordinates.size();
}

double pipe_profile_t::get_length(size_t index) const {
    if (index <= get_point_count() - 1 && index > 0) {
        return coordinates[index] - coordinates[index - 1];
    }
    else {
        return coordinates[get_point_count() - 1] - coordinates[0];
    }
}

void pipe_profile_t::check_parameters() const {
    if (coordinates.empty()) {
        throw std::runtime_error("Нулевой вектор координат");
    }
    if (coordinates.size() != elevations.size()) {
        throw std::runtime_error("Размеры веторов координат и высот не совпадают");
    }
}

// ============================================================================
// pipe_properties_t
// ============================================================================

pipe_properties_t pipe_properties_t::create_pipe(double inner_diameter_val, double wall_thickness_val) {
    pipe_properties_t props;
    props.inner_diameter = inner_diameter_val;
    props.wall_thickness = wall_thickness_val;
    return props;
}

double pipe_properties_t::get_relative_roughness() const {
    if (std::isnan(inner_diameter)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return equivalent_roughness / inner_diameter;
}

double pipe_properties_t::get_outer_diameter() const {
    if (std::isnan(inner_diameter) || std::isnan(wall_thickness)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return inner_diameter + 2.0 * wall_thickness;
}

double pipe_properties_t::get_pipe_area() const {
    if (std::isnan(inner_diameter)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return std::numbers::pi * std::pow(inner_diameter, 2) / 4;
}

void pipe_properties_t::check_parameters() const {
    if (inner_diameter < 0) {
        throw std::runtime_error("Отрицательнный диаметр");
    }
}

// ============================================================================
// oil_properties_t
// ============================================================================

oil_properties_t oil_properties_t::create_oil(double density_val, double kinematic_viscosity_val) {
    oil_properties_t oil;
    oil.density = density_val;
    oil.kinematic_viscosity = kinematic_viscosity_val;
    return oil;
}

double oil_properties_t::calc_kinematic_by_dynamic(double density, double dynamic_viscosity) {
    if (std::isnan(density) || std::isnan(dynamic_viscosity)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return dynamic_viscosity / density;
}

void oil_properties_t::check_parameters() const {
    if (density <= 0 || dynamic_viscosity <= 0) {
        throw std::runtime_error("Некорректные значения плотности и динамической вязкости");
    }
}

// ============================================================================
// pipe_calculator_t
// ============================================================================

double pipe_calculator_t::get_velocity() const {
    if (std::isnan(get_volume_flow_rate())) {
        throw std::runtime_error("Не посчитан объёмный расход");
    }
    if (std::isnan(pipe_properties.get_pipe_area())) {
        throw std::runtime_error("Не посчитана площадь трубы");
    }
    return get_volume_flow_rate() / pipe_properties.get_pipe_area();
}

double pipe_calculator_t::get_reynolds_number() const {
    if (std::isnan(oil_properties.kinematic_viscosity) || std::isnan(pipe_properties.inner_diameter)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (std::isnan(get_velocity())) {
        throw std::runtime_error("Скорость не посчитана");
    }
    return (get_velocity() * pipe_properties.inner_diameter) / oil_properties.kinematic_viscosity;
}

double pipe_calculator_t::get_total_head(size_t index, double pressure) const {
    if (std::isnan(pressure) || std::isnan(oil_properties.density)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (index >= profile.get_point_count()) {
        throw std::runtime_error("Индекс превышает количество точек профиля");
    }
    return pressure / (oil_properties.density * gravity_acceleration) + profile.elevations[index];
}

int pipe_calculator_t::sign_head_difference() const {
    if (std::isnan(get_total_head(0, pressure_start)) || std::isnan(get_total_head(profile.get_point_count() - 1, pressure_end))) {
        throw std::runtime_error("Не задано начальный или конченый полный напор");
    }
    return (get_total_head(0, pressure_start) - get_total_head(profile.get_point_count() - 1, pressure_end) >= 0) ? 1 : -1;
}

double pipe_calculator_t::get_hydraulic_resistance_coefficient() const {
    if (get_reynolds_number() <= 0) {
        throw std::runtime_error("Отрицательное число Рейнольдса");
    }
    if (std::isnan(get_reynolds_number())) {
        if (std::isnan(pipe_properties.get_relative_roughness())) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return 0.11 * std::pow(pipe_properties.get_relative_roughness(), 0.25);
    }
    if (get_reynolds_number() < 2320) {
        return 64.0 / get_reynolds_number();
    }
    return 0.11 * std::pow((68.0 / get_reynolds_number() + pipe_properties.get_relative_roughness()), 0.25);
}

double pipe_calculator_t::get_volume_flow_rate() const {
    if (std::isnan(volume_flow)) {
        if (std::isnan(oil_properties.density) || std::isnan(mass_flow)) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return mass_flow / oil_properties.density;
    }
    else {
        return volume_flow;
    }
}

double pipe_calculator_t::get_diff_elevation(size_t index) const {
    if (index >= profile.get_point_count()) {
        throw std::runtime_error("Индекс превышает количество точек профиля");
    }
    return profile.elevations[index - 1] - profile.elevations[index];
}

double pipe_calculator_t::get_diff_total_head(size_t index) const {
    if (index >= profile.get_point_count()) {
        throw std::runtime_error("Индекс превышает количество точек профиля");
    }
    return get_hydraulic_resistance_coefficient() * 
                (profile.get_length(index) / pipe_properties.inner_diameter) * 
                (std::abs(get_velocity()) * get_velocity()) / (2 * gravity_acceleration);
}

double pipe_calculator_t::get_diff_pressure(size_t index) const {
    if (index >= profile.get_point_count()) {
        throw std::runtime_error("Индекс превышает количество точек профиля");
    }
    return oil_properties.density * gravity_acceleration * (get_diff_total_head(index) - get_diff_elevation(index));
}

double pipe_calculator_t::get_velocity_by_solve_pp() const {
    return sign_head_difference() * std::sqrt((2 * gravity_acceleration * 
        pipe_properties.inner_diameter) / (get_hydraulic_resistance_coefficient() * profile.get_length(0)) * 
        std::abs(get_total_head(0, pressure_start) - get_total_head(profile.get_point_count() - 1, pressure_end)));
}

void pipe_calculator_t::solve_pq() {
    for (size_t i; i < profile.get_point_count() - 1; i++) {
        if (i == 0) {
            pipe_task_result.pressure_profile.push_back(pressure_start);
            pipe_task_result.head_profile.push_back(get_total_head(i, pressure_start));
            continue;
        }
        double pressure_out = pipe_task_result.pressure_profile[i - 1] - get_diff_pressure(i);
        if (pressure_out < 0) {
            throw std::runtime_error("Отрицательное давление");
        }
        pipe_task_result.pressure_profile.push_back(pressure_out);;
        pipe_task_result.head_profile.push_back(get_total_head(i, pressure_out));
    }
}

void pipe_calculator_t::solve_qp() {
    for (size_t i = profile.get_point_count() - 1; i > 0; i--) {
        if (i == profile.get_point_count() - 1) {
            pipe_task_result.pressure_profile.push_back(pressure_end);
            pipe_task_result.head_profile.push_back(get_total_head(i, pressure_end));
            continue;
        }
        double pressure_in = pipe_task_result.pressure_profile[i] + get_diff_pressure(i - 1);
        if (pressure_in < 0) {
            throw std::runtime_error("Отрицательное давление");
        }
        pipe_task_result.pressure_profile.push_back(pressure_in);
        std::rotate(pipe_task_result.pressure_profile.rbegin(), pipe_task_result.pressure_profile.rbegin() + 1, pipe_task_result.pressure_profile.rend());
        pipe_task_result.head_profile.push_back(get_total_head(i - 1, pressure_in));
        std::rotate(pipe_task_result.head_profile.rbegin(), pipe_task_result.head_profile.rbegin() + 1, pipe_task_result.head_profile.rend());
    }
}

void pipe_calculator_t::solve_pp() {
    throw std::runtime_error("Код пока не реализован");
    /* pipe_task_result.volume_flow = get_velocity_by_solve_pp() * pipe_properties.get_pipe_area(); */
}

const pipe_task_result_t& pipe_calculator_t::get_pipe_task_result() const {
    const bool have_profiles = !pipe_task_result.head_profile.empty() && !pipe_task_result.pressure_profile.empty();
    const bool have_flow = std::isfinite(pipe_task_result.volume_flow);
    if (!have_profiles && !have_flow) {
        throw std::runtime_error("Вызов solve_pq() или solve_pp() не произошел");
    }
    return pipe_task_result;
}

// ============================================================================
// Сохранение профилей в формате CSV
// ============================================================================

bool write_head_profile_csv(std::string filepath) {
    if (!is_writable_csv_path(filepath)) return false;

    pipe_profile_t pipe_profile;
    pipe_task_result_t pipe_task_result;

    if (pipe_profile.coordinates.size() != pipe_profile.elevations.size() != pipe_task_result.head_profile.size()) {
        throw std::runtime_error("Несовпадение размерности векторров");
    }

    std::ofstream out(filepath);
    if (!out.is_open()) return false;
    
    out << "x;z;h\n";
    for (size_t i; i < pipe_task_result.pressure_profile.size(); i++) {
        out << pipe_profile.coordinates[i] << ';' << pipe_profile.elevations[i] << ';' << pipe_task_result.head_profile[i] << '\n';
    }
    return out.good();
}

bool write_pressure_profile_csv(std::string filepath) {
    if (!is_writable_csv_path(filepath)) return false;

    pipe_profile_t pipe_profile;
    pipe_task_result_t pipe_task_result;

    if (pipe_profile.coordinates.size() != pipe_profile.elevations.size() != pipe_task_result.pressure_profile.size()) {
        throw std::runtime_error("Несовпадение размерности векторров");
    }

    std::ofstream out(filepath);
    if (!out.is_open()) return false;
    
    out << "x;z;p\n";
    for (size_t i; i < pipe_task_result.pressure_profile.size(); i++) {
        out << pipe_profile.coordinates[i] << ';' << pipe_profile.elevations[i] << ';' << pipe_task_result.pressure_profile[i] << '\n';
    }
    return out.good();
}

} //namespace