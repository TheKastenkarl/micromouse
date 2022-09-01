#include <stdio.h>
#include "robot.h"
#include "maze.h"
#include "API.h"
#include <stdbool.h>
#include "controller.h"
#include "motorEncoders.h"
#include "irSensors.h"
#include "motors.h"
#include "robot.h"



#define ONE_CELL_DISTANCE_RAD 6.5F
#define IS_WALL_THRESHOLD 1200.0F // based on 5-5.5 cm 
#define TARGET_DISTANCE_TO_WALL_LEFT 1800.0F
#define TARGET_DISTANCE_TO_WALL_RIGHT 1800.0F

float target;
bool first_entry;
/**
 * Initilizes the position and orientation of the robot.
 * Assumption: Robot starts in the bottom left corner facing north.
 * 
 * @param robot: Pointer to the robot.
 * @param initial_position: Initial position (= initial cell id) of the robot.
 * @param initial_orientation: Initial orientation (e.g. NORTH) of the robot.
 */
void init_robot(Robot* robot, const int initial_position, const int initial_orientation) {
    robot->position = initial_position;
    robot->orientation = initial_orientation;
}

/**
 * Uses current sensor readings of robot to determine if there is a wall
 * in front of the robot.
 * 
 * @return: 1, if there is a wall, else 0.
 */
int is_wall_front() {
#if SIMULATION
    return API_wallFront();
#else
    if (getFrontIR(1.0F)>IS_WALL_THRESHOLD)
        return 1.0;
    else
        return 0.0;
#endif
}

/**
 * Uses current sensor readings of robot to determine if there is a wall
 * on the right side of the robot.
 * 
 * @return: 1, if there is a wall, else 0.
 */
int is_wall_right() {
#if SIMULATION
    return API_wallRight();
#else
    if (getRightIR(1.0F)>IS_WALL_THRESHOLD)
        return 1.0;
    else
        return 0.0;
#endif
}

/**
 * Uses current sensor readings of robot to determine if there is a wall
 * on the left side of the robot.
 * 
 * @return: 1, if there is a wall, else 0.
 */
int is_wall_left() {
#if SIMULATION
    return API_wallLeft();
#else
    if (getLeftIR(1.0F)>IS_WALL_THRESHOLD)
        return 1.0;
    else
        return 0.0;
#endif
}

/**
 * Uses current sensor readings and the orientation of the robot to determine
 * if there is no wall west of the robot.
 * 
 * @param robot: Pointer to the robot.
 * @return: 1, if there is no wall, else 0.
 */
int no_wall_west(Robot* const robot){
    return (!is_wall_front() && robot->orientation == WEST) ||
           (!is_wall_right() && robot->orientation == SOUTH) ||
           (!is_wall_left() && robot->orientation == NORTH);
}

/**
 * Uses current sensor readings and the orientation of the robot to determine
 * if there is no wall north of the robot.
 * 
 * @param robot: Pointer to the robot.
 * @return: 1, if there is no wall, else 0.
 */
int no_wall_north(Robot* const robot){
    return (!is_wall_front() && robot->orientation == NORTH) ||
           (!is_wall_right() && robot->orientation == WEST) ||
           (!is_wall_left() && robot->orientation == EAST);
}

/**
 * Uses current sensor readings and the orientation of the robot to determine
 * if there is no wall east of the robot.
 * 
 * @param robot: Pointer to the robot.
 * @return: 1, if there is no wall, else 0.
 */
int no_wall_east(Robot* const robot){
    return (!is_wall_front() && robot->orientation == EAST) ||
           (!is_wall_right() && robot->orientation == NORTH) ||
           (!is_wall_left() && robot->orientation == SOUTH);
}

/**
 * Uses current sensor readings and the orientation of the robot to determine
 * if there is no wall south of the robot.
 * 
 * @param robot: Pointer to the robot.
 * @return: 1, if there is no wall, else 0.
 */
int no_wall_south(Robot* const robot){
    return (!is_wall_front() && robot->orientation == SOUTH) ||
           (!is_wall_right() && robot->orientation == EAST) ||
           (!is_wall_left() && robot->orientation == WEST);
}

/**
 * Moves the robot forward. The number of cells which the robot should
 * move forward is specified by "distance".
 * Additionally, the stored position of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 * @param distance: Number of cells the robot should move.
 */
void move_forward(Robot* robot, const int distance) {
    if (robot->orientation == NORTH) {
        robot->position += distance * MAZE_SIZE;
    } else if (robot->orientation == SOUTH) {
        robot->position -= distance * MAZE_SIZE;
    } else if (robot->orientation == EAST) {
        robot->position += distance;
    } else if (robot->orientation == WEST) {
        robot->position -= distance;
    }
#if SIMULATION
    for (int i = 0; i < distance; ++i) {
        API_moveForward();
    }
#else
    
    updateStates();
    
    if (first_entry==false)
    {
        target=positionInRad_L + ONE_CELL_DISTANCE_RAD*(float)distance;
        first_entry=true;
    }
    
    float target_distance_to_wall;
    float distance_to_wall;
    
    if (is_wall_left())
    {
        distance_to_wall = getLeftIR(1.0F);
        target_distance_to_wall = TARGET_DISTANCE_TO_WALL_LEFT;
    }
    else if (is_wall_right())
    {
        distance_to_wall = getRightIR(1.0F);
        target_distance_to_wall = TARGET_DISTANCE_TO_WALL_RIGHT;
    }
    else
    {
        distance_to_wall = 0.0F;
        target_distance_to_wall = 0.0F;
    }
       

	bool isFinished = controlLoop(target,target_distance_to_wall,positionInRad_L,distance_to_wall,velocityInRadPerSample_R,velocityInRadPerSample_L);
    runMotor(differentialWheels.dutyCycle_Right, 1, 0);
    runMotor(differentialWheels.dutyCycle_Left, 0, 0);
//    if (isFinished)
//    {            
//        first_entry=false;
//        return true;  
//    }
//    else 
//        return false;

#endif
}

/**
 * Turns the robot by 90° to the right at the current position (in-place).
 * Additionally, the stored orientation of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 */
void turn_right(Robot* robot) {
    if (robot->orientation != SOUTH) {
        robot->orientation = robot->orientation << 1;
    } else {
        robot->orientation = WEST;
    }
#if SIMULATION
    API_turnRight();
#else
    
    updateStates();
    
    bool isFinished = turnRight(velocityInRadPerSample_R,velocityInRadPerSample_L);
    runMotor(differentialWheels.dutyCycle_Right, 1, 0);
    runMotor(differentialWheels.dutyCycle_Left, 0, 0); 
//    if (isFinished)
//        return true;  
//    else
//        return false;
    
#endif
}

/**
 * Turns the robot by 90° to the left at the current position (in-place).
 * Additionally, the stored orientation of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 */
void turn_left(Robot* robot) {
    if (robot->orientation != WEST) {
        robot->orientation = robot->orientation >> 1;
    } else {
        robot->orientation = SOUTH;
    }
#if SIMULATION
    API_turnLeft();
#else
    
    updateStates();
    
    bool isFinished = turnLeft(velocityInRadPerSample_R,velocityInRadPerSample_L);
    runMotor(differentialWheels.dutyCycle_Right, 1, 0);
    runMotor(differentialWheels.dutyCycle_Left, 0, 0); 
//    if (isFinished)
//        return true;  
//    else
//        return false;
    
#endif
}

/**
 * Turns the robot in-place such that it has the given orientation.
 * Additionally, the stored orientation of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 * @param orientation: Orientation which the robot should have after spinning.
 */
void turn_to_orientation(Robot* robot, const int orientation) {
    if ((robot->orientation << 1 == orientation) || (robot->orientation >> 3 == orientation)) {
        turn_right(robot);
        return;
    } else if ((robot->orientation >> 1 == orientation) || (robot->orientation << 3 == orientation)) {
        turn_left(robot);
        return;
    } else if ((robot->orientation >> 2 == orientation) || (robot->orientation << 2 == orientation)) {
        turn_left(robot);
        turn_left(robot);
        return;
    }
}