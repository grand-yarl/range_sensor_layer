function [E,F,G,constraintSlope,constraintIntercept] = obstacleComputeCustomConstraint(x,detection,obstacle,laneWidth,lanes)
%% Compute custom constraints for the obstacle.

%#codegen
egoX = x(1);
egoY = x(2);

flag = 0;

% Compute constraints only if an obstacle is detected. Otherwsie, set
% constraint to lower road boundary (the inactive constraint).
if detection
    slope1 = ( (obstacle.rlSafeY - egoY)/(obstacle.rlSafeX - egoX) );
    slope2 = -( (obstacle.rrSafeY - egoY)/(obstacle.rrSafeX - egoX) );
    % If ego car is to the left of the obstacle
    if (egoX<=obstacle.rlSafeX)
        % if the ego car is already in the adjacent lane, use the safety
        % zone as the constraint.
        if (egoY>obstacle.rlSafeY)
            constraintSlope = 0;
            constraintIntercept = obstacle.rlSafeY;
        elseif (egoY<obstacle.rrSafeY)
            constraintSlope = 0;
            constraintIntercept = obstacle.rrSafeY;
        elseif ((egoY>obstacle.rrSafeY) && (egoY<obstacle.rlSafeY))
            % The ego car must be above the line formed from the ego car to
            % safe zone corner for left passing.
            if (abs(slope1) <= abs(slope2))
                constraintSlope = tan(atan2(slope1,1));
                constraintIntercept = obstacle.rlSafeY - constraintSlope*obstacle.rlSafeX;
                flag = 0;
            else
                constraintSlope = -tan(atan2(slope2,1));
                constraintIntercept = obstacle.rrSafeY - constraintSlope*obstacle.rrSafeX;
                flag = 1;
            end
        end
    % If the ego car is parallel to the obstacle, use the safety zone as
    % the constraint.
    elseif ( (egoX>obstacle.rlSafeX) && (egoX<=obstacle.flX) )

        
        if (flag == 0)
            constraintSlope = 0;
            constraintIntercept = obstacle.rlSafeY;
        else
            constraintSlope = 0;
            constraintIntercept = obstacle.rrSafeY;
        end 
        
    % If the ego car has passed the obstacle, use the inactive constraint
    % to go back to the center lane.
    else 
        if (flag == 0)
            constraintSlope = 0;
            constraintIntercept = -laneWidth*lanes/2;
        else
            constraintSlope = 0;
            constraintIntercept = laneWidth*lanes/2;
        end
    end
else
    if (flag == 0)
        constraintSlope = 0;
        constraintIntercept = -laneWidth*lanes/2;
    else
        constraintSlope = 0;
        constraintIntercept = laneWidth*lanes/2;
    end
end

%% Define constraint matrices.
if (flag == 0)
    E = [0 0 0 ;0 0 0 ;0 0 0 ];
    F = [0 1 0 0 0 0;0 -1 0 0 0 0;constraintSlope -1 0 0 0 0]; 
    G = [laneWidth*lanes/2;laneWidth*lanes/2;-1*constraintIntercept];
else
    E = [0 0 0 ;0 0 0 ;0 0 0 ];
    F = [0 1 0 0 0 0;0 -1 0 0 0 0;-constraintSlope 1 0 0 0 0]; 
    G = [laneWidth*lanes/2;laneWidth*lanes/2;1*constraintIntercept];
end
