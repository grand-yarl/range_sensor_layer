function [xdot,y] = obstacleVehicleModelCT(x,u)
% The ego car has rectangular shaper with a length of 5 meters and width of
% 2 meters. The model has four states:
%
% * |xPos| - Global horizontal position of the car center
% * |yPos| - Global vertical position of the car center
% * |theta| - Heading angle of the car (0 when facing east, counterclockwise positive)
% * |V| - Speed of the car (positve)
%
% There are two manipulated variables:
%
% * |throttle| - Throttle (positive when accelerating, negative when braking)
% * |delta| - Steering angle change (counterclockwise positive)
%

%#codegen
carLength = 5;
m = 1500;
Iz = 2250;
lf = 2.5;
lr = 2.5;
theta = x(3);
dtheta = x(4);
A = [ 0, 0, 0, 0, cos(theta), -sin(theta);
      0, 0, 0, 0, sin(theta),  cos(theta);
      0, 0, 0, 1,          0,           0;
      0, 0, 0, 0,          0,           0;
      0, 0, 0, 0,          0,           0;
      0, 0, 0, 0,    -dtheta,           0];
B = [  0,     0,      0;          
       0,     0,      0;          
       0,     0,      0;          
       0, lf/Iz, -lr/Iz;          
     1/m,     0,      0;          
       0,   1/m,    1/m];
C = eye(6);
D = zeros(6,4);
xdot = A*x + B*u;
y = C*x + D*u;