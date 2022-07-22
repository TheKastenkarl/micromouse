#include <stdio.h>

#include "API.h"
#include "maze.h"
#include "array.h"
#include "mouse.h"
#include "utils_high_level.h"

void init_maze(Cell maze[MAZE_SIZE][MAZE_SIZE]) {
    int i, j;
    for (i = 0; i < MAZE_SIZE; ++i) {
        for (j = 0; j < MAZE_SIZE; ++j) {
            maze[i][j].walls = 0b1111;
            maze[i][j].visited = 0;
            maze[i][j].cost = MAZE_SIZE * MAZE_SIZE;
            maze[i][j].predecessor_cell_id = -1;
        }
    }
}

Cell* get_cell(Cell maze[MAZE_SIZE][MAZE_SIZE], const int position) {
    int row, col;
    row = position / MAZE_SIZE;
    col = position % MAZE_SIZE;
    return &maze[row][col];
}

// Returns orientation (= positive values) if there is a wall in that orientation, else zero
// Uses stored map to determine answer
int is_wall_in_orientation(Cell maze[MAZE_SIZE][MAZE_SIZE], const int cell_id, const int orientation) {
    Cell* cell = get_cell(maze, cell_id);
    return (cell->walls & orientation);
}

void update_walls(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE]) {
    // WEST
    Cell* current_cell = get_cell(maze, robot->position);
    if (no_wall_west(robot)) {
        current_cell->walls = current_cell->walls & 0b1110; // (~WEST & 0b1111)
        int cell_id_west = get_cell_id_in_orientation(robot->position, maze, WEST);
        Cell* cell_west = get_cell(maze, cell_id_west);
        cell_west->walls = cell_west->walls & 0b1011; // cell in WEST has no wall in EAST
    }
    // NORTH
    if (no_wall_north(robot)) {
        current_cell->walls = current_cell->walls & 0b1101;
        int cell_id_north = get_cell_id_in_orientation(robot->position, maze, NORTH);
        Cell* cell_north = get_cell(maze, cell_id_north);
        cell_north->walls = cell_north->walls & 0b0111; // cell in NORTH has no wall in SOUTH
    }
    // EAST
    if (no_wall_east(robot)) {
        current_cell->walls = current_cell->walls & 0b1011;
        int cell_id_east = get_cell_id_in_orientation(robot->position, maze, EAST);
        Cell* cell_east = get_cell(maze, cell_id_east);
        cell_east->walls = cell_east->walls & 0b1110; // cell in EAST has no wall in WEST
    }
    // SOUTH
    if (no_wall_south(robot)) {
        current_cell->walls = current_cell->walls & 0b0111;
        int cell_id_south = get_cell_id_in_orientation(robot->position, maze, SOUTH);
        Cell* cell_south = get_cell(maze, cell_id_south);
        cell_south->walls = cell_south->walls & 0b1101; // cell in SOUTH has no wall in NORTH
    }

    // Update walls of neighboring cells
}

void update_cell(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE], const float cost, const int predecessor_cell_id) {
    update_walls(robot, maze);
    Cell* cell = get_cell(maze, robot->position);
    cell->visited = 1;
    cell->cost = cost;
    // Set predecessor only if the cell not already has a predecessor
    if (cell->predecessor_cell_id == -1) {
        cell->predecessor_cell_id = predecessor_cell_id;
    }
}

void unvisit_all_cells(Cell maze[MAZE_SIZE][MAZE_SIZE]){
    int i, j;
    for (i = 0; i < MAZE_SIZE; ++i) {
        for (j = 0; j < MAZE_SIZE; ++j) {
            maze[i][j].visited = 0;
            maze[i][j].cost = MAZE_SIZE * MAZE_SIZE;
            maze[i][j].predecessor_cell_id = -1;
        }
    }
}

// Calculates the goal positions of a maze
// Case 1: uneven number of cells -> 1 center cell
// Case 2: even number of cells -> 4 center cells
void calc_goal_cells_ids(int goal_cells_ids[4]) {
    int i;
    // Case 1: uneven number of cells -> 1 center cell
    if (MAZE_SIZE % 2) {
        goal_cells_ids[0] = MAZE_SIZE / 2 * MAZE_SIZE + MAZE_SIZE / 2;
        for (i = 1; i < 4; ++i) {
            goal_cells_ids[i] = -1;
        }       
    } else { // Case 2: even number of cells -> 4 center cells
        goal_cells_ids[0] = (MAZE_SIZE / 2) * MAZE_SIZE + MAZE_SIZE / 2;
        goal_cells_ids[1] = (MAZE_SIZE / 2) * MAZE_SIZE + MAZE_SIZE / 2 - 1;
        goal_cells_ids[2] = (MAZE_SIZE / 2 - 1) * MAZE_SIZE + MAZE_SIZE / 2;
        goal_cells_ids[3] = (MAZE_SIZE / 2 - 1) * MAZE_SIZE + MAZE_SIZE / 2 - 1;
    }
}

// Returns direction from cell with cell_id to neighboring cell with neighboring_cell_id.
// Only works for adjecent cells. If that is not fulfilled, return -1.
// Does not check for borders
int get_orientation_to_neighbor_cell(const int cell_id, const int neighboring_cell_id) {
    if (neighboring_cell_id == cell_id + MAZE_SIZE) {
        return NORTH;
    } else if (neighboring_cell_id == cell_id - MAZE_SIZE) {
        return SOUTH;
    } else if (neighboring_cell_id == cell_id + 1) {
        return EAST;
    } else if (neighboring_cell_id == cell_id - 1) {
        return WEST;
    }
    return -1;
}

// It is not detected if the requested cell would lie outside the maze. Then the return value makes no sense.
int get_cell_id_in_orientation(const int cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], const int orientation) {
    int row = cell_id / MAZE_SIZE;
    int col = cell_id % MAZE_SIZE;

    if (orientation == NORTH) {
        if (row == (MAZE_SIZE - 1)) {
            return -1;
        } else {
            return cell_id + MAZE_SIZE;
        }
    } else if (orientation == SOUTH) {
        if (row == 0) {
            return -1;
        } else {
            return cell_id - MAZE_SIZE;  
        }
    } else if (orientation == EAST) {
        if (col == (MAZE_SIZE - 1)) {
            return -1;
        } else {
            return cell_id + 1; 
        }
    } else if (orientation == WEST) {
        if (col == 0) {
            return -1;
        } else {
            return cell_id - 1; 
        }
    }
}

// It is not detected if the requested cell would lie outside the maze. Then the return value makes no sense.
int get_cell_id_in_direction(const int current_orientation, const int cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], const int direction) {
    // Cell in NORTH
    if (((current_orientation == NORTH) && (direction == FRONT)) ||
        ((current_orientation == WEST) && (direction == RIGHT)) ||
        ((current_orientation == EAST) && (direction == LEFT))) {
        return get_cell_id_in_orientation(cell_id, maze, NORTH);
    // Cell in SOUTH
    } else if (((current_orientation == SOUTH) && (direction == FRONT)) ||
        ((current_orientation == WEST) && (direction == LEFT)) ||
        ((current_orientation == EAST) && (direction == RIGHT))) {
        return get_cell_id_in_orientation(cell_id, maze, SOUTH);
    // Cell in EAST
    } else if (((current_orientation == EAST) && (direction == FRONT)) ||
        ((current_orientation == NORTH) && (direction == RIGHT)) ||
        ((current_orientation == SOUTH) && (direction == LEFT))) {
        return get_cell_id_in_orientation(cell_id, maze, EAST);
    // Cell in WEST
    } else if (((current_orientation == WEST) && (direction == FRONT)) ||
        ((current_orientation == SOUTH) && (direction == RIGHT)) ||
        ((current_orientation == NORTH) && (direction == LEFT))) {
        return get_cell_id_in_orientation(cell_id, maze, WEST);
    }
}