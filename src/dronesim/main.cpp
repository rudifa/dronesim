#include "dronesim.h"

int main()
{
    const double time_step = 0.1;
    const double total_time = 60.0;
    const size_t num_steps = static_cast<size_t>(total_time / time_step);

    auto drone = std::make_unique<Drone>(1.5, 0.1, 1.225, 1.0, 0.1);
    auto speed_controller = std::make_unique<PIDController>(0.5, 0.1, 0.2);
    auto altitude_controller = std::make_unique<PIDController>(0.2, 0, 0);
    auto results = std::make_unique<SimulationResults>(num_steps);

    for (size_t i = 0; i < num_steps; ++i)
    {
        double current_time = i * time_step;
        double target_altitude = get_target_altitude(current_time);

        double altitude_error = target_altitude - drone->get_altitude();
        double speed_reference = altitude_controller->compute(altitude_error, time_step);
        speed_reference = std::max(-MAX_VELOCITY, std::min(speed_reference, MAX_VELOCITY));

        double speed_error = speed_reference - drone->get_velocity();
        double thrust_adjustment = speed_controller->compute(speed_error, time_step);
        drone->set_thrust(drone->get_thrust() + thrust_adjustment);

        double acceleration = drone->update(time_step);
        results->update(i, current_time, *drone, acceleration, target_altitude);
    }

    results->print();

    return 0;
}
