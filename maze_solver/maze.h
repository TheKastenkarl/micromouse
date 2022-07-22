#pragma once

#include "mouse.h"

// Number of columns (= number of rows) of maze
#define MAZE_SIZE 16 

// Define if simulation or real robot is used
#define SIMULATION 1

// Assumption: Mouse start always in the bottom left corner facing north.
// Positions are described with numbers from 0 to MAZE_SIZE * MAZE_SIZE
// Example: cell in third row, fourth column -> position = (3 - 1) * MAZE_SIZE + (4 - 1)

typedef struct Cell {
    int walls; // West (0b0001), North (0b0010), East (0b0100), South (0b1000)
    int visited;
    float cost;
    int predecessor_cell_id;
} Cell;

void init_maze(Cell maze[MAZE_SIZE][MAZE_SIZE]);

Cell* get_cell(Cell maze[MAZE_SIZE][MAZE_SIZE], const int position);

// Returns orientation (= positive values) if there is a wall in that orientation, else zero
// Uses stored map to determine answer
int is_wall_in_orientation(Cell maze[MAZE_SIZE][MAZE_SIZE], const int cell_id, const int orientation);

void update_walls(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE]);

void update_cell(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE], const float cost, const int predecessor_cell_id);

void unvisit_all_cells(Cell maze[MAZE_SIZE][MAZE_SIZE]);

// Calculates the goal positions of a maze
// Case 1: uneven number of cells -> 1 center cell
// Case 2: even number of cells -> 4 center cells
void calc_goal_cells_ids(int goal_cells_ids[4]);

// Returns direction from cell with cell_id to neighboring cell with neighboring_cell_id.
// Only works for adjecent cells. If that is not fulfilled, return -1.
int get_orientation_to_neighbor_cell(const int cell_id, const int neighboring_cell_id);

// It is not detected if the requested cell would lie outside the maze. Then the return value makes no sense.
int get_cell_id_in_orientation(const int cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], const int orientation);

// It is not detected if the requested cell would lie outside the maze. Then the return value makes no sense.
int get_cell_id_in_direction(const int current_orientation, const int cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], const int direction);