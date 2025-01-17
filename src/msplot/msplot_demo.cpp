#include "msplot.hpp"
#include <cmath>
#include <vector>
#include <iostream>

// Function to generate a sawtooth wave
double sawtooth(double x, double period = 2 * M_PI) {
    return 2 * (x / period - std::floor(0.5 + x / period));
}

// Function to generate a triangle wave
double triangle(double x, double period = 2 * M_PI) {
    return 2 * std::abs(sawtooth(x + period / 4, period)) - 1;
}

int main()
{
    // Generate sample data
    std::vector<double> time_array;
    std::vector<double> altitude_array;
    std::vector<double> target_altitude_array;
    std::vector<double> velocity_array;
    std::vector<double> sawtooth_array;
    std::vector<double> triangle_array;

    for (int i = 0; i < 200; ++i)
    {
        double time = i * 0.1; // time in seconds
        time_array.push_back(time);
        altitude_array.push_back(1000 * std::sin(time)); // example altitude data
        target_altitude_array.push_back(1000);           // constant target altitude

        // Calculate velocity as the derivative of altitude
        velocity_array.push_back(1000 * std::cos(time)); // derivative of sin is cos

        // Generate sawtooth and triangle waves
        sawtooth_array.push_back(500 * sawtooth(time) + 500); // Scale to 0-1000 range
        triangle_array.push_back(500 * triangle(time) + 500); // Scale to 0-1000 range
    }

    MSPlot::Figure fig(800, 800); // Acommodate 4 subplots

    // Create 4 subplots vertically

    fig.addSubplot(4, 1, 0); // Altitude
    fig.plot(time_array, altitude_array, "Altitude (m)", Color(Color::Blue));
    fig.plot(time_array, target_altitude_array, "Target Altitude", Color(Color::Red));

    fig.addSubplot(4, 1, 1); // Velocity
    fig.plot(time_array, velocity_array, "Velocity (m/s)", Color(Color::Green));

    fig.addSubplot(4, 1, 2); // Sawtooth
    fig.plot(time_array, sawtooth_array, "Sawtooth Wave",  Color(Color::Purple));

    fig.addSubplot(4, 1, 3); // Triangle
    fig.plot(time_array, triangle_array, "Triangle Wave", Color(Color::Orange));


    const std::string filename = "msplot.svg";

    if (fig.save(filename))
    {
        std::cerr << "MSPlot saved as " << filename << std::endl;
        system(("open " + filename).c_str()); // Open the file in the default browser
    }
    else
    {
        std::cerr << "Failed to save MSPlot as " << filename << std::endl;
    }

    return 0;
}
