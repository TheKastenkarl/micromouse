#pragma once

#include "mouse.h"
#include "maze.h"
#include "array.h"

void log_position_walls_predecessor(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE]);

/*
- drive forward, if there is no wall and the cell has not been visited yet
- drive left, if there is no wall and the cell has not been visited yet
- drive right, if there is no wall and the cell has not been visited yet
- drive backward (= turn around and drive forward), if there is no wall and the cell has not been visited yet -> not required as this cell has always been visited before
- if none of the upper actions is possible, go back to the predecessor cell of the current cell -> use stack for that with indices / pointers to cells
- We visited all cells if the robot is back again at the starting position.
*/
void exploration_dfs(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE]);

int exploit_bfs(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE], int goals_cell_ids[4]);

void explore_and_exploit();

// returns updated cell_sequence
// cell_sequence needs size MAZE_SIZE * MAZE_SIZE and must be initialization with zeros: "int cell_sequence[MAZE_SIZE * MAZE_SIZE];"
// return number of elements in path (remaining array remains filled with zeros)
int reconstruct_path_to_goal(const int goal_cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], int cell_sequence[MAZE_SIZE * MAZE_SIZE]);

// returns updated cell_sequence
// cell_sequence needs size MAZE_SIZE * MAZE_SIZE and must be initialization with zeros: "int cell_sequence[MAZE_SIZE * MAZE_SIZE];"
// Array action sequence is modified in-place and it must have size "num_path_elements - 1"
void reconstruct_orientation_sequence_from_cell_sequence(Cell maze[MAZE_SIZE][MAZE_SIZE], int cell_sequence[MAZE_SIZE * MAZE_SIZE], const int num_path_elements, Array* orientation_sequence);

void generate_actions_from_orientation_sequence(Robot* robot, Array orientation_sequence);