#include <stdio.h>

#include "maze_solver.h"
#include "queue.h"
#include "API.h"

/**
 * Logs the current position of the robot, the walls at the current position
 * and the predecessor cell of the current cell.
 * Only for simulation environment.
 * 
 * @param robot: Pointer to the robot.
 * @param maze: Pointer to the maze.
 */
#if SIMULATION
void log_position_walls_predecessor(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE]){
    char str[10];
    API_log("Position:");
    sprintf(str, "%d", robot->position);
    API_log(str); 
    API_log("Predecessor:");
    sprintf(str, "%d", get_cell(maze, robot->position)->predecessor_cell_id);
    API_log(str);
    API_log("Walls:");
    sprintf(str, "%d", get_cell(maze, robot->position)->walls);
    API_log(str);
    API_log("-------------------");
}
#endif

/**
 * The provided robot explores the provided maze with a depth-first-search:
 * - drive forward, if there is no wall and the cell has not been visited yet
 * - drive left, if there is no wall and the cell has not been visited yet
 * - drive right, if there is no wall and the cell has not been visited yet
 * - if none of the upper actions is possible, go back to the predecessor cell of the current cell
 * - We visited all cells if the robot is back again at the starting position.
 * 
 * After the search, all walls are mapped and written back to the passed adress for "maze".
 * The robot has the same orientation after the search as at the beginning of the search.
 * 
 * Assumption: Mouse starts in the bottom left corner facing north.
 * 
 * @param robot: Pointer to the robot.
 * @param maze: Pointer to the maze.
 */
void exploration_dfs(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE]) {
    int starting_position = robot->position;
    int starting_orientation = robot->orientation;
    int predecessor_cell_id = -1;
    do {
        update_cell(robot, maze, predecessor_cell_id);
        predecessor_cell_id = robot->position;
#if SIMULATION
        log_position_walls_predecessor(robot, maze);
#endif

        if (!is_wall_front() && !get_cell(maze, get_cell_id_in_direction(robot->orientation, robot->position, maze, FRONT))->visited) {
            move_forward(robot, 1);
        } else if (!is_wall_left() && !get_cell(maze, get_cell_id_in_direction(robot->orientation, robot->position, maze, LEFT))->visited) {
            spin_left(robot);
            move_forward(robot, 1);
        } else if (!is_wall_right() && !get_cell(maze, get_cell_id_in_direction(robot->orientation, robot->position, maze, RIGHT))->visited) {
            spin_right(robot);
            move_forward(robot, 1);
        } else { // go back to predecessor cell
            predecessor_cell_id = get_cell(maze, robot->position)->predecessor_cell_id;
            spin_to_orientation(robot, get_orientation_to_neighbor_cell(robot->position, predecessor_cell_id));
            move_forward(robot, 1);
        }
#if SIMULATION
        mark_cell(robot->position);
#endif
    }
    while (robot->position != starting_position);
    
    // Turn robot such that it has the same orientation like at the beginning
    spin_to_orientation(robot, starting_orientation);
}

/**
 * Breadth-first-search used to find the shortest path from the given start cell
 * to one of the goal cells provided in "goals_cell_ids".
 * 
 * Writes the shortest path from start to goal as an array of cell ids back to the
 * provided adress of "cell_sequence", e.g. [0,1,17,16,...,119].
 * 
 * @param start_cell_id: Starting cell id for the shortest path search.
 * @param maze: Pointer to the maze.
 * @param goals_cell_ids: Array with cell id(s) of goal cell(s).
 * @param cell_sequence: Empty, initialized array, used to store the result.
 * @return: Cell id of goal which is closest to the start cell. If goal is not found, return -1.
 */
int shortest_path_bfs(const int start_cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], int goals_cell_ids[4], Array* cell_sequence) {
    int current_cell_id;
    Cell* current_cell;
    int neighbor_cell_id;
    Cell* neighbor_cell;
    int orientation;
    int goal_cell;
    int goal_found = 0;

    node_t* head = NULL;
    enqueue(&head, start_cell_id);

    // Step 1: Find the shortest path to goal with a bfs and store for every cell the predecessor cell
    while (!goal_found) {
        current_cell_id = dequeue(&head);
        if (current_cell_id == -1) {
            return -1; // failure, goal not found
        }
        current_cell = get_cell(maze, current_cell_id);

        // Orientations (NORTH, ...) are encoded as integers
        for (orientation = WEST; orientation <= SOUTH; orientation = 2 * orientation) {
            neighbor_cell_id = get_cell_id_in_orientation(current_cell_id, maze, orientation);
            neighbor_cell = get_cell(maze, neighbor_cell_id);
            if ((neighbor_cell_id != -1) && !neighbor_cell->visited && !is_wall_in_orientation(maze, current_cell_id, orientation)) {
                neighbor_cell = get_cell(maze, neighbor_cell_id);
                neighbor_cell->visited = 1;
                neighbor_cell->predecessor_cell_id = current_cell_id;
                if (is_present(neighbor_cell_id, goals_cell_ids, 4)) {
                    goal_cell = neighbor_cell_id;
                    goal_found = 1;
                    break;
                }
                enqueue(&head, neighbor_cell_id);
            }
        }
    }

    // Step 2: Use the stored predecessor cells to reconstruct the path from the start cell to the goal cell
    reconstruct_cell_sequence_to_goal(goal_cell, maze, cell_sequence);
    return goal_cell;
}

/**
 * Explores the map in a first step and then uses the stored map to take the shortest path from
 * start to goal.
 * 
 * Assumption: Mouse starts in the bottom left corner facing north.
 */
void explore_and_exploit() {
    Robot robot;
    int num_path_elements;
    int goal_cell_id;
    Cell maze[MAZE_SIZE][MAZE_SIZE];

    int goal_cells_ids[4] = {0};
    Array cell_sequence;
    Array orientation_sequence;

    init_robot(&robot);
    init_maze(maze);

    init_array(&cell_sequence, 4 * MAZE_SIZE);
    init_array(&orientation_sequence, 4 * MAZE_SIZE);
    calc_goal_cells_ids(goal_cells_ids);

    exploration_dfs(&robot, maze);
    unvisit_all_cells(maze);

    shortest_path_bfs(robot.position, maze, goal_cells_ids, &cell_sequence);
    visualize_cell_sequence(&cell_sequence);
    reconstruct_orientation_sequence_from_cell_sequence(maze, &cell_sequence, &orientation_sequence);

    generate_actions_from_orientation_sequence(&robot, &orientation_sequence);
    free_array(&orientation_sequence);
    free_array(&cell_sequence);
}

/**
 * Uses the stored predecessor cells after a shortest path search to reconstruct the
 * cell id sequence from the start cell to the goal cell.
 * 
 * Writes the shortest path from start to goal as an array of cell ids back to the
 * provided adress of "cell_sequence", e.g. [0,1,17,16,...,119].
 * 
 * @param goal_cell_id: Goal cell id which was determined in the shortest path search.
 * @param maze: Pointer to the maze.
 * @param cell_sequence: Empty, initialized array, used to store the result.
 */
void reconstruct_cell_sequence_to_goal(const int goal_cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], Array* cell_sequence) {
    int cell_id = goal_cell_id;
    Cell* cell = get_cell(maze, cell_id);
    int i;

    for (i = 0; i < MAZE_SIZE * MAZE_SIZE; ++i) {
        insert_array(cell_sequence, cell_id);
        if (cell_id == 0) { // reached starting position
            break;
        }
        cell_id = cell->predecessor_cell_id;
        cell = get_cell(maze, cell_id);
    }
    reverse_array(cell_sequence);
}

/**
 * Uses the sequence of cell ids of neighboring cells to reconstruct the corresponding sequence of orientations,
 * e.g. the sequence [NORTH,NORTH,WEST,NORTH] means that the robot has to move to the cell in the north,
 * then to the cell in the NORTH, then to the cell in the WEST, ...
 * 
 * Writes the result (array of orientations) back to the provided adress of "orientation_sequence".
 * 
 * @param maze: Pointer to the maze.
 * @param cell_sequence: Sequence of cell ids.
 * @param orientation_sequence: Empty, initialized array, used to store the result.
 */
void reconstruct_orientation_sequence_from_cell_sequence(Cell maze[MAZE_SIZE][MAZE_SIZE], Array* cell_sequence, Array* orientation_sequence) {
    int i;
    int orientation;

    for (i = 0; i < cell_sequence->used - 1; ++i) {
        orientation = get_orientation_to_neighbor_cell(cell_sequence->array[i], cell_sequence->array[i + 1]);
        insert_array(orientation_sequence, orientation);
    }
}

/**
 * Uses the a sequence of orientations, e.g. [NORTH,NORTH,WEST,NORTH], to generate a corresponding sequence
 * of actions for the robot, e.g. [turn_left(), move_forward(2), ...].
 *  
 * @param maze: Pointer to the robot.
 * @param orientation_sequence: Sequence of orientations.
 */
void generate_actions_from_orientation_sequence(Robot* robot, Array* const orientation_sequence) {
    int i;
    int num_repeated;
    for (i = 0; i < orientation_sequence->used; i = i + num_repeated) {
        spin_to_orientation(robot, orientation_sequence->array[i]);
        // count how often a orientation is repeated such that the robot does not have to stop moving after one cell if the next cell is in the same orientation
        for (num_repeated = 1; ((i + num_repeated) < orientation_sequence->used) && (orientation_sequence->array[i] == orientation_sequence->array[i + num_repeated]); ++num_repeated);
        move_forward(robot, num_repeated);
    }
}

/**
 * Determines if an element is present in an array.
 *  
 * @param element: Element of interest.
 * @param arr: Pointer to the array.
 * @param arr_length: Length of array.
 * @return: 1 if element is present, else 0.
 */
int is_present(const int element, int* arr, const int arr_length) {
    int i;
    for (i = 0; i < arr_length; ++i) {
        if (arr[i] == element) {
            return 1;
        }
    }
    return 0;
}

/**
 * Visualizes the specified cell sequence in the simulation in blue.
 * In each cell, the distance to the last cell of the cell sequence is written.
 * 
 * @param cell_sequence: Cell sequence which should be visualized.
 */
#if SIMULATION
void visualize_cell_sequence(Array* cell_sequence) {
    int i, row, col, remaining_distance;
    for (i = 0; i < cell_sequence->used; ++i) {
        row = cell_sequence->array[i] / MAZE_SIZE;
        col = cell_sequence->array[i] % MAZE_SIZE;
        remaining_distance = cell_sequence->used - 1 - i;
        char str[5];
        sprintf(str, "%d", remaining_distance);
        API_setText(col, row, str);
        API_setColor(col, row, 'B');
    }
}
#endif

/**
 * Marks the specified cell in the simulation in green and writes
 * the cell id on it.
 * 
 * @param cell_id: Cell id which should be marked.
 */
#if SIMULATION
void mark_cell(const int cell_id) {
    int row = cell_id / MAZE_SIZE;
    int col = cell_id % MAZE_SIZE;
    char str[5];
    sprintf(str, "%d", cell_id);
    API_setText(col, row, str);
    API_setColor(col, row, 'G');
}
#endif