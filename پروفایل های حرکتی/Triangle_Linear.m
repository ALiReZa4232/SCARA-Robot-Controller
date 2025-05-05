
clc
clear
close all
% Define the parameters
mode = 0;
ss = 1; % step size in mm
acc = 500; % acceleration in mm/(s*s)
speed = 643.65463342017;
deltaS = 200; % distance to travel

if mode == 1
acc = speed * speed / deltaS;
end

% Calculate the number of steps
absSteps = round(abs(deltaS) / ss);
s_1 = deltaS / 2;

% Initialize arrays to store velocity and time
velocity = zeros(1, absSteps);
time = zeros(1, absSteps);

% Calculate velocity for each step
for incStep = 1:absSteps
    s = (incStep - 0.5) * ss;
    if s < s_1
        velocity(incStep) = sqrt(2 * s * acc);
    else
        velocity(incStep) = sqrt(2 * (deltaS - s) * acc);
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
title('Triangle Velocity Profile');
xlabel('Time (sec)');
ylabel('Velocity (mm/s)');
grid on;