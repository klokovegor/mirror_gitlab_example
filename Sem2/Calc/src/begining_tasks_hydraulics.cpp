#include "begining_tasks_hydraulics.h"



/// @brief Ускорение свободного падения Земли
double gravity_accel_m_s2 = 9.81;
/// @brief Эквивалентная шероховатость (принятая константа модели)
double equivalent_roughness_m = 1e-4;

double length_si(double length_km) {
    return length_km * 1000;
}

double diameter_si(double diameter_mm) {
    return diameter_mm * 1e-3;
}

double pressure_si(double pressure_value, std::string unit) {
    if (unit == "кПа") {
        return pressure_value * 1e3;
    }
    if (unit == "кгс/см^2") {
        return pressure_value * 98066.5;
    }
    return pressure_value; // Па или неизвестная единица
}

double dynamic_viscosity_si(double dynamic_viscosity_cp) {
    return dynamic_viscosity_cp * 1e-2;
}

double kinematic_viscosity_si(double kinematic_viscosity_cst) {
    return kinematic_viscosity_cst * 1e-2;
}

double reynolds_number_calc(double velocity_m_s, double diameter_m, double kinematic_viscosity_m2_s) {
    return (velocity_m_s * diameter_m) / kinematic_viscosity_m2_s;
}

double total_head_calc(double pressure_pa, double density_kg_m3, double elevation_m) {
    return pressure_pa / (density_kg_m3 * gravity_accel_m_s2) + elevation_m;
}

int sign(double a, double b) {
    if ((a - b) > 0) {
        return 1;
    }
    else {
        return -1;
    }
}

double epsilon(double equivalent_roughness_m, double diameter_m) {
    return equivalent_roughness_m / diameter_m;
}

double lambda_calc(double reynolds_number, double relative_roughness) {
    if (reynolds_number == 0) {
        return 0.11 * std::pow(relative_roughness, 0.25);
    }
    if (reynolds_number < 2320) {
        return 64 / reynolds_number;
    }
    return 0.11 * (68 / reynolds_number + relative_roughness);
}

double pipe_area_calc(double diameter_m) {
    return (M_PI * diameter_m * diameter_m) / 4;
}

double kinematic_viscosity_calc(double dynamic_viscosity_pa_s, double density_kg_m3) {
    return dynamic_viscosity_pa_s / density_kg_m3;
}

double q_calc(double mass_flow_kg_s, double density_kg_m3) {
    return mass_flow_kg_s / density_kg_m3;
}

double qp_task(double length_m, double diameter_m, double elevation_start_m, double elevation_end_m,
                double pressure_end_pa, double mass_flow_kg_s, double density_kg_m3, double dynamic_viscosity_pa_s) {
    // Уравнение энергии в форме Дарси–Вейсбаха: перепад давлений = вклад трения + вклад геодезической разности.
    const double Q = q_calc(mass_flow_kg_s, density_kg_m3);
    const double velocity_m_s = (4 * Q) / (diameter_m * diameter_m * M_PI);

    const double kinematic_viscosity_m2_s = kinematic_viscosity_calc(dynamic_viscosity_pa_s, density_kg_m3);
    const double reynolds_number = reynolds_number_calc(velocity_m_s, diameter_m, kinematic_viscosity_m2_s);
    const double lambda = lambda_calc(reynolds_number, epsilon(equivalent_roughness_m, diameter_m));

    const double h_friction = lambda * (length_m / diameter_m) *
                               ((std::abs(velocity_m_s) * velocity_m_s) / (2 * gravity_accel_m_s2));
    const double delta_p = density_kg_m3 * gravity_accel_m_s2 * (h_friction - (elevation_start_m - elevation_end_m));

    return pressure_end_pa + delta_p;
}

double pp_task(double length_m, double diameter_m, double elevation_start_m, double elevation_end_m,
                double pressure_start_pa, double pressure_end_pa, double density_kg_m3, double kinematic_viscosity_m2_s) {
    (void)kinematic_viscosity_m2_s; // В текущей модели коэффициент трения выбирается по фиксированной аппроксимации.

    const double head_start_m = total_head_calc(pressure_start_pa, density_kg_m3, elevation_start_m);
    const double head_end_m = total_head_calc(pressure_end_pa, density_kg_m3, elevation_end_m);

    const int sign_head_start_minus_end = sign(head_start_m, head_end_m);
    const double lambda = lambda_calc(0, epsilon(equivalent_roughness_m, diameter_m));

    const double velocity_m_s = sign_head_start_minus_end *
                                 std::sqrt((2 * gravity_accel_m_s2 * diameter_m) / (lambda * length_m) *
                                           std::abs(head_start_m - head_end_m));

    const double pipe_area_m2 = pipe_area_calc(diameter_m);
    const double mass_flow_kg_s = density_kg_m3 * velocity_m_s * pipe_area_m2;

    return mass_flow_kg_s;
}

std::vector<std::vector<double>> pressure_profile(double pressure_start_pa, double pressure_end_pa, double length_m, int step_intervals) {
    std::vector<std::vector<double>> out;

    const double l_step = length_m / step_intervals;
    const double p_step = (pressure_end_pa - pressure_start_pa) / step_intervals;

    for (int j = 0; j <= step_intervals; ++j) {
        std::vector<double> charr = {l_step * j, pressure_start_pa + p_step * j};
        out.push_back(charr);
    }

    return out;
}

bool write_pressure_profile_csv(const std::string& filepath, const std::vector<std::vector<double>>& pressure_prof, int step_intervals) {
    std::ofstream out(filepath);
    if (!out) return false;

    if (pressure_prof.size() < static_cast<size_t>(step_intervals + 1)) {
        return false;
    }

    out << "x;";
    for (int j = 0; j <= step_intervals; ++j) {
        const std::vector<double>& charr = pressure_prof[j];
        out << charr[0];
        if (j != step_intervals) {
            out << ';';
        }
    }
    out << "\n";

    // Вторая строка: все значения P
    out << "P;";
    for (int j = 0; j <= step_intervals; ++j) {
        const std::vector<double>& charr = pressure_prof[j];
        out << charr[1];
        if (j != step_intervals) {
            out << ';';
        }
    }
    out << "\n";

    return true;
}

std::vector<std::vector<double>> head_profile(double pressure_start_pa, double pressure_end_pa, double length_m, int step_intervals,
                                                double density_kg_m3, double elevation_start_m, double elevation_end_m) {
    std::vector<std::vector<double>> out;

    std::vector<std::vector<double>> pressure_prof = pressure_profile(pressure_start_pa, pressure_end_pa, length_m, step_intervals);

    const double z_step = (elevation_end_m - elevation_start_m) / step_intervals;
    for (int j = 0; j <= step_intervals; ++j) {
        double elevation_m = elevation_start_m + z_step * j;
        double head_m = total_head_calc(pressure_prof[j][1], density_kg_m3, elevation_m);
        std::vector<double> charr = {pressure_prof[j][0], head_m};
        out.push_back(charr);
    }

    return out;
}

bool write_head_profile_csv(const std::string& filepath, const std::vector<std::vector<double>>& head_prof, int step_intervals) {
    std::ofstream out(filepath);
    if (!out) return false;

    if (head_prof.size() < static_cast<size_t>(step_intervals + 1)) {
        return false;
    }

    out << "x;";
    for (int j = 0; j <= step_intervals; ++j) {
        const std::vector<double>& charr = head_prof[j];
        out << charr[0];
        if (j != step_intervals) {
            out << ';';
        }
    }
    out << "\n";

    // Вторая строка: все значения H
    out << "H;";
    for (int j = 0; j <= step_intervals; ++j) {
        const std::vector<double>& charr = head_prof[j];
        out << charr[1];
        if (j != step_intervals) {
            out << ';';
        }
    }
    out << "\n";

    return true;
}





