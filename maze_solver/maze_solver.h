#ifndef MAZE_SOLVER_H
#define	MAZE_SOLVER_H

#include "robot.h"
#include "maze.h"
#include "array.h"

#if SIMULATION
void log_position_walls_predecessor(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE]);
#endif

void exploration_dfs(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE]);

int shortest_path_bfs(const int start_cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], int goals_cell_ids[4], Array* cell_sequence);

void explore_and_exploit();

void reconstruct_cell_sequence_to_goal(const int goal_cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], Array* cell_sequence);

void reconstruct_orientation_sequence_from_cell_sequence(Cell maze[MAZE_SIZE][MAZE_SIZE], Array* cell_sequence, Array* orientation_sequence);

void generate_actions_from_orientation_sequence(Robot* robot, Array* const orientation_sequence);

int is_present(const int element, int* arr, const int arr_length);

#if SIMULATION
void visualize_cell_sequence(Array* cell_sequence);
#endif

#if SIMULATION
void mark_cell(const int cell_id);
#endif

#endif /* MAZE_SOLVER_H */