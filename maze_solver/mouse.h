#pragma once

typedef struct Robot {
    int position;
    int orientation;
} Robot;

enum Direction {
    LEFT = 0,
    FRONT = 1,
    RIGHT = 2,
    BACK = 3
};

enum Orientation {
    WEST = 0b0001,
    NORTH = 0b0010,
    EAST = 0b0100,
    SOUTH = 0b1000
};

void init_robot(Robot* robot);

int wall_front();
int wall_right();
int wall_left();

int no_wall_west(Robot* const robot);
int no_wall_north(Robot* const robot);
int no_wall_east(Robot* const robot);
int no_wall_south(Robot* const robot);

void move_forward(Robot* robot, const int distance);

// spin at current position by -90°
void turn_right(Robot* robot);

// spin at current position by +90°
void turn_left(Robot* robot);

// turn the robot such that it has the given orientation
void turn_to_orientation(Robot* robot, const int orientation);