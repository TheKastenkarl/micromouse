#include <stdio.h>
#include "robot.h"
#include "maze.h"
#include "API.h"

/**
 * Initilizes the position and orientation of the robot.
 * 
 * Assumption: Robot starts in the bottom left corner facing north.
 * 
 * @param robot: Pointer to the robot.
 */
void init_robot(Robot* robot) {
    robot->position = 0;
    robot->orientation = NORTH;
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
    // TODO: Add low-level function of micromouse here
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
    // TODO: Add low-level function of micromouse here
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
    // TODO: Add low-level function of micromouse here
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
    // TODO: Add low-level function of micromouse here
#endif
}

/**
 * Spins the robot by 90° to the right at the current position.
 * Additionally, the stored orientation of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 */
void spin_right(Robot* robot) {
    if (robot->orientation != SOUTH) {
        robot->orientation = robot->orientation << 1;
    } else {
        robot->orientation = WEST;
    }
#if SIMULATION
    API_turnRight();
#else
    // TODO: Add low-level function of micromouse here
#endif
}

/**
 * Spins the robot by 90° to the left at the current position.
 * Additionally, the stored orientation of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 */
void spin_left(Robot* robot) {
    if (robot->orientation != WEST) {
        robot->orientation = robot->orientation >> 1;
    } else {
        robot->orientation = SOUTH;
    }
#if SIMULATION
    API_turnLeft();
#else
    // TODO: Add low-level function of micromouse here
#endif
}

/**
 * Spins the robot such that it has the given orientation.
 * Additionally, the stored orientation of the robot is updated.
 * 
 * @param robot: Pointer to the robot.
 * @param orientation: Orientation which the robot should have after spinning.
 */
void spin_to_orientation(Robot* robot, const int orientation) {
    if ((robot->orientation << 1 == orientation) || (robot->orientation >> 3 == orientation)) {
        spin_right(robot);
        return;
    } else if ((robot->orientation >> 1 == orientation) || (robot->orientation << 3 == orientation)) {
        spin_left(robot);
        return;
    } else if ((robot->orientation >> 2 == orientation) || (robot->orientation << 2 == orientation)) {
        spin_left(robot);
        spin_left(robot);
        return;
    }
}