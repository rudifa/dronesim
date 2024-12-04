#! python

import numpy as np
import matplotlib.pyplot as plt

# Constants
g = 9.81  # Acceleration due to gravity (m/s^2)
MAX_THRUST = 30.0  # Maximum thrust (N)
MAX_VELOCITY = 50.0  # Maximum velocity (m/s)

# Drone parameters
mass = 1.5  # Mass of the drone (kg)
rotor_area = 0.1  # Rotor area (m^2)
air_density = 1.225  # Air density at sea level (kg/m^3)
C_L = 1.0  # Lift coefficient (assumed constant for simplicity)
C_D = 0.1  # Drag coefficient (assumed constant for simplicity)

# Desired altitude
target_altitude = 100.0  # Target altitude to hover at (m)

# Functions to calculate forces


def calculate_lift(thrust):
    """Calculate lift force based on thrust."""
    return thrust


def calculate_weight(mass):
    """Calculate weight of the drone."""
    return mass * g


def calculate_drag(velocity):
    """Calculate drag force based on velocity."""
    return C_D * 0.5 * air_density * rotor_area * velocity**2


# Desired speed
target_speed = 10.0  # Target speed to achieve (m/s)


# Simulation parameters
time_step = 0.1  # Time step for simulation (s)
total_time = 30.0  # Total time for simulation (s)
num_steps = int(total_time / time_step)

# Initialize arrays to store results
time_array = np.linspace(0, total_time, num_steps)
altitude_array = np.zeros(num_steps)
velocity_array = np.zeros(num_steps)
thrust_array = np.zeros(num_steps)  # Array to store thrust values
acceleration_array = np.zeros(num_steps)  # Array to store acceleration values

# Initial conditions
thrust = calculate_weight(mass)  # Initial thrust equal to weight (N)
altitude = 0.0  # Initial altitude (m)
velocity = 0.0  # Initial velocity (m/s)

# PID controller parameters for speed control
K_p_speed = 0.5  # Proportional gain for speed
K_i_speed = 0.1  # Integral gain for speed
K_d_speed = 0.2  # Derivative gain for speed


# Altitude control parameters
K_p_altitude = 0.2  # Proportional gain for altitude

# Initialize error variables
integral_error_speed = 0
previous_error_speed = 0

# Simulation loop
for i in range(1, num_steps):
    lift = calculate_lift(thrust)
    weight = calculate_weight(mass)

    # Altitude control logic
    altitude_error = target_altitude - altitude
    speed_reference = K_p_altitude * altitude_error

    # Limit the speed reference
    speed_reference = max(min(speed_reference, MAX_VELOCITY), -MAX_VELOCITY)

    # Speed control logic
    speed_error = speed_reference - velocity
    integral_error_speed += speed_error * time_step
    derivative_error_speed = (speed_error - previous_error_speed) / time_step

    thrust_adjustment = (K_p_speed * speed_error +
                         K_i_speed * integral_error_speed +
                         K_d_speed * derivative_error_speed)
    thrust += thrust_adjustment

    # Limit thrust to be between a minimum (e.g., 0) and MAX_THRUST
    thrust = max(min(thrust, MAX_THRUST), 0)

    previous_error_speed = speed_error

    thrust_array[i] = thrust  # Store current thrust value

    # Calculate net force and acceleration
    net_force = lift - weight - calculate_drag(velocity)
    acceleration = net_force / mass
    acceleration_array[i] = acceleration  # Store current acceleration value

    # Update velocity and altitude
    velocity += acceleration * time_step
    altitude += velocity * time_step

    # Store results
    velocity_array[i] = velocity
    altitude_array[i] = altitude

# Plotting results
plt.figure(figsize=(12, 10))

plt.subplot(4, 1, 1)
plt.plot(time_array, altitude_array, label='Altitude (m)', color='blue')
plt.axhline(y=target_altitude, color='red',
            linestyle='--', label='Target Altitude')
plt.title('Drone Flight Simulation with Thrust Control')
plt.xlabel('Time (s)')
plt.ylabel('Altitude (m)')
plt.grid()
plt.legend()

plt.subplot(4, 1, 2)
plt.plot(time_array, velocity_array, label='Velocity (m/s)', color='red')
plt.xlabel('Time (s)')
plt.ylabel('Velocity (m/s)')
plt.grid()
plt.legend()

plt.subplot(4, 1, 3)
plt.plot(time_array, thrust_array, label='Thrust (N)', color='green')
plt.xlabel('Time (s)')
plt.ylabel('Thrust (N)')
plt.grid()
plt.legend()

plt.subplot(4, 1, 4)
plt.plot(time_array, acceleration_array,
         label='Acceleration (m/s²)', color='purple')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s²)')
plt.grid()
plt.legend()

plt.tight_layout()
plt.show()
