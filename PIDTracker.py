import bpy
import math
from random import random

FIRST_FRAME =           0
LAST_FRAME =            250
max_acceleration =      30.0 + (random() - 0.5) * 3
gravity =               [0, 0, -9.81]
pid_cumulative_error =  [0, 0, 0]
pid_last_error =        [0, 0, 0]

def vector_diff(vector_1, vector_2, lower_bound = -999, upper_bound = 999, gain = 1):
    res = [0, 0, 0]
    res[0] = gain * max(min(vector_1[0] - vector_2[0], upper_bound), lower_bound)
    res[1] = gain * max(min(vector_1[1] - vector_2[1], upper_bound), lower_bound)
    res[2] = gain * max(min(vector_1[2] - vector_2[2], upper_bound), lower_bound)
    return res

def vector_add(vector_1, vector_2 , vector_3 = [0,0,0], v1_gain = 1, v2_gain = 1, v3_gain = 3, lower_bound = -999, upper_bound = 999, gain = 1):
    res = [0, 0, 0]
    res[0] = gain * max(min(v1_gain * vector_1[0] + v2_gain * vector_2[0] + v3_gain * vector_3[0], upper_bound), lower_bound)
    res[1] = gain * max(min(v1_gain * vector_1[1] + v2_gain * vector_2[1] + v3_gain * vector_3[1], upper_bound), lower_bound)
    res[2] = gain * max(min(v1_gain * vector_1[2] + v2_gain * vector_2[2] + v3_gain * vector_3[2], upper_bound), lower_bound)
    return res

def pid(kp, ki, kd, setpoint, position, integral_error, previous_error, dt, lower_bound = -999, upper_bound = 999):
    ret =               0
    error =             setpoint - position
    integral_error +=   error * dt
    derivative_error =  (error - previous_error) / dt
    
    ret += kp * error               # Proportional
    ret += ki * integral_error    # Integral
    ret += kd * derivative_error    # Derivative
    
    return (max(min(ret, upper_bound), lower_bound), integral_error, error)  #bounded return
    
def main():
    # Get scene data
    scene = bpy.context.scene
    scene.frame_start = FIRST_FRAME
    scene.frame_end = LAST_FRAME
    dt = 1/scene.render.fps
    
    # Get object and target position
    object = bpy.data.objects.get("Cube")
    target = bpy.data.objects.get("Empty")
    
    # Switch to a class, location, velocity and acceleration of the object
    seek_location =         [0, 0, 0]
    seek_velocity =         [0, 0, 0]
    seek_acceleration =     [0, 0, 0]
    
    target_location =       [1, 0, 10]
    target_velocity =       [1, 2, 0]
    target_acceleration =   [0, 0, 0]
    
    # Set initial conditions
    object.location = seek_location
    target.location = target_location
    power = 5
    
    # PID config
    kp = 15
    ki = 1
    kd = 2.5
    
    for i in range(FIRST_FRAME, LAST_FRAME):
        object.keyframe_insert(data_path = "location", frame = i)
        target.keyframe_insert(data_path = "location", frame = i)
        
        # PID
        seek_acceleration[0], pid_cumulative_error[0], pid_last_error[0] = pid(kp, ki, kd, target_location[0], seek_location[0], pid_cumulative_error[0], pid_last_error[0], dt, -max_acceleration, max_acceleration)
        seek_acceleration[1], pid_cumulative_error[1], pid_last_error[1] = pid(kp, ki, kd, target_location[1], seek_location[1], pid_cumulative_error[1], pid_last_error[1], dt, -max_acceleration, max_acceleration)
        seek_acceleration[2], pid_cumulative_error[2], pid_last_error[2] = pid(kp, ki, kd, target_location[2], seek_location[2], pid_cumulative_error[2], pid_last_error[2], dt, -max_acceleration, max_acceleration)
        
        # Add gravity, should be done before maybe but idc
        seek_acceleration[2] += -gravity[2]
        
        # Update velocities and accelerations
        seek_velocity =     vector_add(seek_velocity, gravity, seek_acceleration, 1, dt, dt)
        target_velocity =   vector_add(target_velocity, target_acceleration, v1_gain = 1, v2_gain = dt)
        seek_location =     vector_add(seek_location, seek_velocity, v1_gain = 1, v2_gain = dt)
        target_location =   vector_add(target_location, target_velocity, v1_gain = 1, v2_gain = dt)
        
        # Store location
        object.location = seek_location
        target.location = target_location
    
if __name__ == "__main__":
    main()
