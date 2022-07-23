#ifndef MAZE_H
#define	MAZE_H

#include "robot.h"
#include "array.h"

// Number of columns (= number of rows) of maze
#define MAZE_SIZE 16 

// Define if simulation or real robot is used
#define SIMULATION 1

// Define if in simulation visualization should be used
#define VISUALIZATION 1

typedef struct Cell {
    int walls; // West (0b0001), North (0b0010), East (0b0100), South (0b1000)
    int visited;
    int predecessor_cell_id;
} Cell;

void init_maze(Cell maze[MAZE_SIZE][MAZE_SIZE]);

Cell* get_cell(Cell maze[MAZE_SIZE][MAZE_SIZE], const int cell_id);

int is_wall_in_orientation(Cell maze[MAZE_SIZE][MAZE_SIZE], const int cell_id, const int orientation);

void update_walls(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE]);

void update_cell(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE], const int predecessor_cell_id);

void unvisit_all_cells(Cell maze[MAZE_SIZE][MAZE_SIZE]);

void calc_goal_cells_ids(int goal_cells_ids[4]);

int get_orientation_to_neighbor_cell(const int cell_id, const int neighboring_cell_id);

int get_cell_id_in_orientation(const int cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], const int orientation);

int get_cell_id_in_direction(const int current_orientation, const int cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], const int direction);

#if SIMULATION && VISUALIZATION
void visualize_cell_sequence(Array* cell_sequence);
#endif

#if SIMULATION && VISUALIZATION
void mark_cell(const int cell_id);
#endif

#endif /* MAZE_H */