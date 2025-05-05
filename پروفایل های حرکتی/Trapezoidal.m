clc
clear
close all
% Define the parameters
ss = 1; % step size in mm
vmax = 400; % maximum desired linear velocity in mm/s
acc = 200; % acceleration in mm/(s*s)
deltaS = 348.281; % distance to travel

% Calculate the number of steps
absSteps = round(abs(deltaS) / ss);

% Define end point of acceleration and start point of deceleration
s_1 = vmax^2 / (2 * acc);
s_2 = deltaS - s_1;

if s_1 > s_2 % if we don't even reach full speed
    s_1 = deltaS / 2;
    s_2 = deltaS / 2;
    vmax = sqrt(deltaS * acc);
end

% Initialize arrays to store velocity and time
velocity = zeros(1, absSteps);
time = zeros(1, absSteps);

% Calculate velocity for each step
for incStep = 1:absSteps
    s = (incStep - 0.5) * ss;
    if s < s_1
        velocity(incStep) = sqrt(2 * s * acc);
    elseif s < s_2
        velocity(incStep) = vmax;
    else
        velocity(incStep) = sqrt(vmax^2 - 2 * (s - s_2) * acc);
    end
    time(incStep) = incStep;
end

% Plot the velocity profile
stairs(time, velocity);
title('Trapezoidal Velocity Profile');
xlabel('Time (steps)');
ylabel('Velocity (mm/s)');
grid on;
