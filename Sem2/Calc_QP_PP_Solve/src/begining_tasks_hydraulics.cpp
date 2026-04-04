#include "begining_tasks_hydraulics.h"

namespace hydraulics_struct {

namespace {

bool is_windows_absolute_path(const std::string& filepath) {
    return filepath.size() >= 3 &&
           std::isalpha(static_cast<unsigned char>(filepath[0])) &&
           filepath[1] == ':' &&
           (filepath[2] == '\\' || filepath[2] == '/');
}

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

}  // namespace

// ============================================================================
// Свободные функции
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
        {"Па*с", [](double x) { return x; }},
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
    if (coordinates.size() != elevations.size()) {
        return 0;
    }
    return coordinates.size();
}

double pipe_profile_t::get_length() const {
    if (coordinates.empty()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return coordinates.back() - coordinates.front();
}

// ============================================================================
// pipe_properties_t
// ============================================================================

pipe_properties_t pipe_properties_t::create_pipe(double length_val, double inner_diameter_val, double wall_thickness_val,
                                                     double elevation_start, double elevation_end) {
    pipe_properties_t props;
    props.length = length_val;
    props.inner_diameter = inner_diameter_val;
    props.wall_thickness = wall_thickness_val;
    props.start_elevation = elevation_start;
    props.end_elevation = elevation_end;
    return props;
}

double pipe_properties_t::get_relative_roughness() const {
    if (inner_diameter <= 0) {
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
    if (inner_diameter <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return M_PI * std::pow(inner_diameter, 2) / 4;
}

// ============================================================================
// oil_properties_t
// ============================================================================

oil_properties_t oil_properties_t::create_oil(double density_val, double kinematic_viscosity_val,
                                                   double dynamic_viscosity_val) {
    oil_properties_t oil;
    oil.density = density_val;
    oil.kinematic_viscosity = kinematic_viscosity_val;
    oil.dynamic_viscosity = dynamic_viscosity_val;
    return oil;
}

double oil_properties_t::get_kinematic_viscosity() const {
    if (density <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return dynamic_viscosity / density;
}

double oil_properties_t::get_dynamic_viscosity() const {
    if (std::isnan(density) || std::isnan(kinematic_viscosity)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return kinematic_viscosity * density;
}


// ============================================================================
// pipe_task_result_t
// ============================================================================

double pipe_task_result_t::get_reynolds_number() const {
    if (oil_properties.kinematic_viscosity <= 0 || pipe_properties.inner_diameter <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return (get_velocity() * pipe_properties.inner_diameter) / oil_properties.kinematic_viscosity;
}

double pipe_task_result_t::get_start_total_head() const {
    if (oil_properties.density <= 0 || oil_properties.density == std::numeric_limits<double>::quiet_NaN()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return pipe_properties.pressure_start / (oil_properties.density * gravity_acceleration) + pipe_properties.start_elevation;
}

double pipe_task_result_t::get_end_total_head() const {
    if (oil_properties.density <= 0 || oil_properties.density == std::numeric_limits<double>::quiet_NaN()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return pipe_properties.pressure_end / (oil_properties.density * gravity_acceleration) + pipe_properties.end_elevation;
}

int pipe_task_result_t::sign_difference() const {
    if (std::isnan(get_start_total_head()) || std::isnan(get_end_total_head())) {
        return 0;
    }
    return (get_start_total_head() - get_end_total_head() >= 0) ? 1 : -1;
}

double pipe_task_result_t::get_hydraulic_resistance_coefficient() const {
    // Задача PP: массовый расход ещё неизвестен — число Рейнольдса из скорости недоступно;
    // используем асимптотику турбулентного режима по относительной шероховатости (как при Re → ∞).
    if (std::isnan(pipe_properties.mass_flow)) {
        const double rel = pipe_properties.get_relative_roughness();
        if (std::isnan(rel) || rel <= 0) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return 0.11 * std::pow(rel, 0.25);
    }

    const double re = get_reynolds_number();
    if (std::isnan(re) || re <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (re < 2320) {
        return 64.0 / re;
    }
    return 0.11 * std::pow((68.0 / re + pipe_properties.get_relative_roughness()), 0.25);
}

double pipe_task_result_t::get_volume_flow_rate() const {
    if (oil_properties.density <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return pipe_properties.mass_flow / oil_properties.density;
}

double pipe_task_result_t::get_velocity() const {
    if (std::isnan(get_volume_flow_rate()) || std::isnan(pipe_properties.inner_diameter) || pipe_properties.inner_diameter <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return (4 * get_volume_flow_rate()) / (std::pow(pipe_properties.inner_diameter, 2) * M_PI);
}

// ============================================================================
// pipe_calculator_t
// ============================================================================

double pipe_calculator_t::solve_pq() const{
    
    const double elevation_diff = pipe_properties.start_elevation - pipe_properties.end_elevation;
    
    const double h_friction = pipe_task_result.get_hydraulic_resistance_coefficient() * 
                (pipe_properties.length / pipe_properties.inner_diameter) * 
                (std::abs(pipe_task_result.get_velocity()) * pipe_task_result.get_velocity()) / (2 * gravity_acceleration);

    const double delta_p = oil_properties.density * gravity_acceleration * 
                           (h_friction - elevation_diff);

    return pipe_properties.pressure_end + delta_p;
}

double pipe_calculator_t::solve_pp() const {

    double H1 = pipe_task_result.get_start_total_head();
    double H2 = pipe_task_result.get_end_total_head();
    double lambda = pipe_task_result.get_hydraulic_resistance_coefficient();
    double diameter = pipe_properties.inner_diameter;
    double length = pipe_properties.length;
    
    if (std::isnan(H1) || std::isnan(H2) || std::isnan(lambda) || 
        std::isnan(diameter) || std::isnan(length) || diameter <= 0 || length <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    
    int sign = pipe_task_result.sign_difference();
    double head_diff = std::abs(H1 - H2);
    
    double velocity = sign * std::sqrt((2 * gravity_acceleration * diameter) / 
                                       (lambda * length) * head_diff);
    
    double area = pipe_properties.get_pipe_area();
    if (std::isnan(area)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    
    return oil_properties.density * velocity * area;
}

// ============================================================================
// Функции для работы с профилями
// ============================================================================

std::vector<std::vector<double>> head_profile(
    pipe_calculator_t& calculator,
    pipe_profile_t& profile)
{
    std::vector<std::vector<double>> out;
    
    if (profile.get_point_count() <= 0 || std::isnan(profile.get_length()) || profile.get_length() <= 0) {
        return out;
    }
    
    double step_full_head = (calculator.pipe_task_result.get_start_total_head() - 
                    calculator.pipe_task_result.get_end_total_head()) / profile.get_point_count();
    
    for (size_t i = 0; i < profile.get_point_count(); i++) {
        
        const double head = calculator.pipe_task_result.get_start_total_head() - step_full_head * i;
        
        out.push_back({profile.coordinates[i], profile.elevations[i], head});
    }
    
    return out;
}

bool write_head_profile_csv(
    const std::string& filepath, 
    const std::vector<std::vector<double>>& head_prof)
{
    if (!is_writable_csv_path(filepath)) return false;

    std::ofstream out(filepath);
    if (!out.is_open()) return false;
    
    out << "x;z;H\n";
    for (const auto& point : head_prof) {
        if (point.size() >= 3) {
            out << point[0] << ';' << point[1] << ';' << point[2] << '\n';
        }
    }
    return out.good();
}

std::vector<std::vector<double>> pressure_profile(
    pipe_calculator_t& calculator,
    pipe_profile_t& profile)
{
    std::vector<std::vector<double>> out;
    
    if (profile.get_point_count() <= 0 || std::isnan(profile.get_length()) || profile.get_length() <= 0) {
        return out;
    }
    
    for (const auto& point : head_profile(calculator, profile)) {
        if (point.size() < 3) continue;
        
        const double x = point[0];
        const double z = point[1];
        const double h = point[2];
        
        const double press = (h - z) * calculator.oil_properties.density * gravity_acceleration;
        
        out.push_back({x, z, press});
    }
    
    return out;
}

bool write_pressure_profile_csv(
    const std::string& filepath, 
    const std::vector<std::vector<double>>& pressure_prof)
{
    if (!is_writable_csv_path(filepath)) return false;

    std::ofstream out(filepath);
    if (!out.is_open()) return false;
    
    out << "x;z;p\n";
    for (const auto& point : pressure_prof) {
        if (point.size() >= 3) {
            out << point[0] << ';' << point[1] << ';' << point[2] << '\n';
        }
    }
    return out.good();
}

}