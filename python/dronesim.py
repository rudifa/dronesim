#!/Users/rudifarkas/opt/anaconda3/bin/python

import numpy as np
import matplotlib.pyplot as plt

# Constants
g = 9.81  # Acceleration due to gravity (m/s^2)
MAX_THRUST = 30.0  # Maximum thrust (N)
MAX_VELOCITY = 50.0  # Maximum velocity (m/s)


class Drone:
    def __init__(self, mass, rotor_area, air_density, C_L, C_D):
        self.mass = mass
        self.rotor_area = rotor_area
        self.air_density = air_density
        self.C_L = C_L
        self.C_D = C_D
        self.altitude = 0.0
        self.velocity = 0.0
        self.thrust = self.calculate_weight()

    def calculate_lift(self):
        """Calculate lift force based on thrust."""
        return self.thrust

    def calculate_weight(self):
        """Calculate weight of the drone."""
        return self.mass * g

    def calculate_drag(self):
        """Calculate drag force based on velocity."""
        return self.C_D * 0.5 * self.air_density * self.rotor_area * self.velocity**2

    def update(self, time_step):
        lift = self.calculate_lift()
        weight = self.calculate_weight()
        drag = self.calculate_drag()

        net_force = lift - weight - drag
        acceleration = net_force / self.mass

        self.velocity += acceleration * time_step
        self.altitude += self.velocity * time_step

        return acceleration

    def set_thrust(self, thrust):
        self.thrust = max(min(thrust, MAX_THRUST), 0)


class PIDController:
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.integral = 0
        self.previous_error = 0

    def compute(self, error, time_step):
        self.integral += error * time_step
        derivative = (error - self.previous_error) / time_step
        output = (self.kp * error +
                  self.ki * self.integral +
                  self.kd * derivative)
        self.previous_error = error
        return output


class SimulationResults:
    def __init__(self, num_steps):
        self.time_array = np.zeros(num_steps)
        self.altitude_array = np.zeros(num_steps)
        self.velocity_array = np.zeros(num_steps)
        self.thrust_array = np.zeros(num_steps)
        self.acceleration_array = np.zeros(num_steps)
        self.target_altitude_array = np.zeros(
            num_steps)  # New array for target altitude

    def update(self, i, time, drone, acceleration, target_altitude):  # Added target_altitude parameter
        self.time_array[i] = time
        self.altitude_array[i] = drone.altitude
        self.velocity_array[i] = drone.velocity
        self.thrust_array[i] = drone.thrust
        self.acceleration_array[i] = acceleration
        # Store target altitude
        self.target_altitude_array[i] = target_altitude

    def plot(self):
        plt.figure(figsize=(12, 10))

        plt.subplot(4, 1, 1)
        plt.plot(self.time_array, self.altitude_array,
                 label='Altitude (m)', color='blue')
        plt.plot(self.time_array, self.target_altitude_array,
                 color='red', linestyle='--', label='Target Altitude')
        plt.title('Drone Flight Simulation with PID Control')
        plt.xlabel('Time (s)')
        plt.ylabel('Altitude (m)')
        plt.grid()
        plt.legend()

        plt.subplot(4, 1, 2)
        plt.plot(self.time_array, self.velocity_array,
                 label='Velocity (m/s)', color='red')
        plt.xlabel('Time (s)')
        plt.ylabel('Velocity (m/s)')
        plt.grid()
        plt.legend()

        plt.subplot(4, 1, 3)
        plt.plot(self.time_array, self.thrust_array,
                 label='Thrust (N)', color='green')
        plt.xlabel('Time (s)')
        plt.ylabel('Thrust (N)')
        plt.grid()
        plt.legend()

        plt.subplot(4, 1, 4)
        plt.plot(self.time_array, self.acceleration_array,
                 label='Acceleration (m/s²)', color='purple')
        plt.xlabel('Time (s)')
        plt.ylabel('Acceleration (m/s²)')
        plt.grid()
        plt.legend()

        plt.tight_layout()
        plt.show()


# Simulation parameters
time_step = 0.1  # Time step for simulation (s)
total_time = 60.0  # Total time for simulation (s)
num_steps = int(total_time / time_step)


# Create drone and controllers
drone = Drone(mass=1.5, rotor_area=0.1, air_density=1.225, C_L=1.0, C_D=0.1)
speed_controller = PIDController(kp=0.5, ki=0.1, kd=0.2)
altitude_controller = PIDController(kp=0.2, ki=0, kd=0)
results = SimulationResults(num_steps)


# Target altitude to hover at (m)
def get_target_altitude(time):
    return 100.0 if time <= 30.0 else 40.0


# Simulation loop
for i in range(num_steps):
    current_time = i * time_step

    # Get target altitude for current time
    target_altitude = get_target_altitude(current_time)

    # Altitude control
    altitude_error = target_altitude - drone.altitude
    speed_reference = altitude_controller.compute(altitude_error, time_step)
    speed_reference = max(min(speed_reference, MAX_VELOCITY), -MAX_VELOCITY)

    # Speed control
    speed_error = speed_reference - drone.velocity
    thrust_adjustment = speed_controller.compute(speed_error, time_step)
    drone.set_thrust(drone.thrust + thrust_adjustment)

    # Update drone state
    acceleration = drone.update(time_step)

    # Store results
    results.update(i, current_time, drone, acceleration, target_altitude)

# Plot results
results.plot()
