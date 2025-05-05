clc
clear
close all
% Define the parameters
ss = 1; % step size in mm
vmax = 2000; % maximum desired linear velocity in mm/s
acc = sqrt(3) * vmax; % acceleration in mm/(s*s)
deltaS = 3200; % distance to travel

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

% Calculate velocity and time for each step
for incStep = 1:absSteps
    s = (incStep - 0.5) * ss;
    if s < s_1
        velocity(incStep) = sqrt(2 * s * acc);
    elseif s < s_2
         velocity(incStep) = vmax;
    else
        velocity(incStep) = sqrt(vmax^2 - 2 * (s - s_2) * acc);
    end

    % Calculate the time for each step
    if incStep == 1
        time(incStep) = sqrt(2 * ss / acc);
    else
        time(incStep) = time(incStep - 1) + ss / velocity(incStep - 1);
    end
end

% Plot the velocity profile
stairs(time, velocity);
title('Trapezoidal Velocity Profile');
xlabel('Time (s)');
ylabel('Velocity (mm/s)');
grid on;
