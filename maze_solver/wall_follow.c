#include "wall_follow.h"
#include "API.h"
#include "maze.h"

void wall_follow() {
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "abc");
    while (1) {
        if (!API_wallLeft()) {
            API_turnLeft();
        }
        while (API_wallFront()) {
            API_turnRight();
        }
        API_moveForward();
    }
}

void wall_follow_2() {
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "abc");
    Robot robot;
    init_robot(&robot);
    logging("Initialization finished");
    while (1) {
        if (!wall_left()) {
            turn_left(&robot);
        }
        while (wall_front()) {
            turn_right(&robot);
        }
        move_forward(&robot,1);
    }
}