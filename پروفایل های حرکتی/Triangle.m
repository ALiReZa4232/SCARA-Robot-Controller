clc
clear
close all
% Define the parameters
ss = 1; % step size in mm
acc = 200; % acceleration in mm/(s*s)
deltaS = 200; % distance to travel

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
    time(incStep) = incStep;
end

% Plot the velocity profile
stairs(time, velocity);
title('Triangle Velocity Profile');
xlabel('Time (step)');
ylabel('Velocity (mm/s)');
grid on;
