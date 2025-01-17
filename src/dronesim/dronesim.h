#ifndef DRONESIM_H
#define DRONESIM_H

#include <iostream>
#include <memory>
#include <vector>

#include "../msplot/msplot.hpp"

const double g = 9.81;
const double MAX_THRUST = 30.0;
const double MAX_VELOCITY = 50.0;

class Drone
{
   private:
    double mass;
    double rotor_area;
    double air_density;
    double C_L;
    double C_D;
    double altitude;
    double velocity;
    double thrust;

   public:
    Drone(double mass, double rotor_area, double air_density, double C_L,
          double C_D);
    double calculate_lift();
    double calculate_weight();
    double calculate_drag();
    double update(double time_step);
    void set_thrust(double new_thrust);

    double get_altitude() const;
    double get_velocity() const;
    double get_thrust() const;
};

class PIDController
{
   private:
    double kp;
    double ki;
    double kd;
    double integral;
    double previous_error;

   public:
    PIDController(double kp, double ki, double kd);
    double compute(double error, double time_step);
};

class SimulationResults
{
   private:
    std::vector<double> time_array;
    std::vector<double> altitude_array;
    std::vector<double> velocity_array;
    std::vector<double> thrust_array;
    std::vector<double> acceleration_array;
    std::vector<double> target_altitude_array;

   public:
    SimulationResults(size_t num_steps);
    void update(size_t i, double time, const Drone &drone, double acceleration,
                double target_altitude);

    void print() const;
    void plot() const;
};

double get_target_altitude(double time);

#endif  // DRONESIM_H
