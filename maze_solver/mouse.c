#include <stdio.h>
#include "mouse.h"
#include "maze.h"
#include "API.h"

void init_robot(Robot* robot) {
    robot->position = 0;
    robot->orientation = NORTH;
}

int wall_front() {
#if SIMULATION
    return API_wallFront();
#else
    // TODO: Add low-level function of micromouse here
#endif
}

int wall_right() {
#if SIMULATION
    return API_wallRight();
#else
    // TODO: Add low-level function of micromouse here
#endif
}

int wall_left() {
#if SIMULATION
    return API_wallLeft();
#else
    // TODO: Add low-level function of micromouse here
#endif
}

int no_wall_west(Robot* const robot){
    return (!wall_front() && robot->orientation == WEST) ||
           (!wall_right() && robot->orientation == SOUTH) ||
           (!wall_left() && robot->orientation == NORTH);
}

int no_wall_north(Robot* const robot){
    return (!wall_front() && robot->orientation == NORTH) ||
           (!wall_right() && robot->orientation == WEST) ||
           (!wall_left() && robot->orientation == EAST);
}

int no_wall_east(Robot* const robot){
    return (!wall_front() && robot->orientation == EAST) ||
           (!wall_right() && robot->orientation == NORTH) ||
           (!wall_left() && robot->orientation == SOUTH);
}

int no_wall_south(Robot* const robot){
    return (!wall_front() && robot->orientation == SOUTH) ||
           (!wall_right() && robot->orientation == EAST) ||
           (!wall_left() && robot->orientation == WEST);
}

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
    API_moveForward();
    int row = robot->position / MAZE_SIZE;
    int col = robot->position % MAZE_SIZE;
    char str_position[5];
    sprintf(str_position, "%d", robot->position);
    API_setText(col, row, str_position);
    API_setColor(col, row, 'G');
#else
    // TODO: Add low-level function of micromouse here
#endif
}

// spin at current position by -90°
void turn_right(Robot* robot) {
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

// spin at current position by +90°
void turn_left(Robot* robot) {
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

// turn the robot such that it has the given orientation
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