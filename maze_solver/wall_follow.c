#include "wall_follow.h"
#include "robot.h"

/**
 * Simple wall following algorithm where the robot always follows the left wall.
 * The robot might end up in a loop without finding the goal.
 */
void wall_follow() {
    Robot robot;
    init_robot(&robot, 0, NORTH);
    while (1) {
        if (!is_wall_left()) {
            turn_left(&robot);
        }
        while (is_wall_front()) {
            turn_right(&robot);
        }
        move_forward(&robot,1);
    }
}