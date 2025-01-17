#include "dronesim.h"

#include <algorithm>
#include <cmath>

Drone::Drone(double mass, double rotor_area, double air_density, double C_L,
             double C_D)
    : mass(mass),
      rotor_area(rotor_area),
      air_density(air_density),
      C_L(C_L),
      C_D(C_D),
      altitude(0.0),
      velocity(0.0)
{
    thrust = calculate_weight();
}

double Drone::calculate_lift() { return thrust; }

double Drone::calculate_weight() { return mass * g; }

double Drone::calculate_drag()
{
    return C_D * 0.5 * air_density * rotor_area * pow(velocity, 2);
}

double Drone::update(double time_step)
{
    double lift = calculate_lift();
    double weight = calculate_weight();
    double drag = calculate_drag();

    double net_force = lift - weight - drag;
    double acceleration = net_force / mass;

    velocity += acceleration * time_step;
    altitude += velocity * time_step;

    return acceleration;
}

void Drone::set_thrust(double new_thrust)
{
    thrust = std::max(0.0, std::min(new_thrust, MAX_THRUST));
}

double Drone::get_altitude() const { return altitude; }
double Drone::get_velocity() const { return velocity; }
double Drone::get_thrust() const { return thrust; }

PIDController::PIDController(double kp, double ki, double kd)
    : kp(kp), ki(ki), kd(kd), integral(0), previous_error(0)
{
}

double PIDController::compute(double error, double time_step)
{
    integral += error * time_step;
    double derivative = (error - previous_error) / time_step;
    double output = kp * error + ki * integral + kd * derivative;
    previous_error = error;
    return output;
}

SimulationResults::SimulationResults(size_t num_steps)
{
    time_array.resize(num_steps);
    altitude_array.resize(num_steps);
    velocity_array.resize(num_steps);
    thrust_array.resize(num_steps);
    acceleration_array.resize(num_steps);
    target_altitude_array.resize(num_steps);
}

void SimulationResults::update(size_t i, double time, const Drone &drone,
                               double acceleration, double target_altitude)
{
    time_array[i] = time;
    altitude_array[i] = drone.get_altitude();
    velocity_array[i] = drone.get_velocity();
    thrust_array[i] = drone.get_thrust();
    acceleration_array[i] = acceleration;
    target_altitude_array[i] = target_altitude;
}

void SimulationResults::print() const
{
    std::cout << "Time\tAltitude\tVelocity\tThrust\tAcceleration\tTarget Alt\n";
    for (size_t i = 0; i < time_array.size(); ++i)
    {
        std::cout << time_array[i] << "\t" << target_altitude_array[i] << "\t\t"
                  << altitude_array[i] << "\t\t" << velocity_array[i] << "\t\t"
                  << thrust_array[i] << "\t" << acceleration_array[i] << "\n";
    }
}

double get_target_altitude(double time) { return time <= 30.0 ? 100.0 : 40.0; }

void SimulationResults::plot() const
{
    MSPlot::Figure fig(800, 800);  // Acommodate 4 subplots

    // Create 5 subplots vertically

    fig.addSubplot(5, 1, 0);  // Target Altitude
    fig.plot(time_array, target_altitude_array, Color(Color::Red));
    fig.title("Target Altitude (m)");
    fig.xlabel("Time (s)");

    fig.addSubplot(5, 1, 1);  // Altitude
    fig.plot(time_array, altitude_array, Color(Color::Blue));
    fig.title("Altitude (m)");
    fig.xlabel("Time (s)");

    fig.addSubplot(5, 1, 2);  // Velocity
    fig.plot(time_array, velocity_array, Color(Color::Green));
    fig.title("Velocity (m/s)");
    fig.xlabel("Time (s)");

    fig.addSubplot(5, 1, 3);  // Thrust
    fig.plot(time_array, thrust_array, Color(Color::Purple));
    fig.title("Thrust (N)");
    fig.xlabel("Time (s)");

    fig.addSubplot(5, 1, 4);  // Accelleration
    fig.plot(time_array, acceleration_array, Color(Color::Orange));
    fig.title("Accelleration (m/s²)");
    fig.xlabel("Time (s)");

    const std::string filename = "dronesim.svg";

    if (fig.save(filename))
    {
        std::cerr << "Simulation plot saved as " << filename << std::endl;
        system(("open " + filename)
                   .c_str());  // Open the file in the default browser
    }
    else
    {
        std::cerr << "Failed to save Simulation plot as " << filename
                  << std::endl;
    }
}
