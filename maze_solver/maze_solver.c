#include <stdio.h>
#include "maze_solver.h"
#include "queue.h"
#include "API.h"
#include "utils_high_level.h"

void log_position_walls_predecessor(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE]){
    char str[10];
    logging("Position");
    sprintf(str, "%d", robot->position);
    logging(str); 
    sprintf(str, "%d", get_cell(maze, robot->position)->predecessor_cell_id);
    logging("Predecessor");
    logging(str);
    logging("Walls");
    sprintf(str, "%d", get_cell(maze, robot->position)->walls);
    logging(str);
    logging(" ");
}

/*
- drive forward, if there is no wall and the cell has not been visited yet
- drive left, if there is no wall and the cell has not been visited yet
- drive right, if there is no wall and the cell has not been visited yet
- drive backward (= turn around and drive forward), if there is no wall and the cell has not been visited yet -> not required as this cell has always been visited before
- if none of the upper actions is possible, go back to the predecessor cell of the current cell -> use stack for that with indices / pointers to cells
- We visited all cells if the robot is back again at the starting position.
*/
void exploration_dfs(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE]) {
    int starting_position = robot->position;
    int predecessor_cell_id = -1;
    do {
        update_cell(robot, maze, MAZE_SIZE * MAZE_SIZE, predecessor_cell_id);
        predecessor_cell_id = robot->position;
        log_position_walls_predecessor(robot, maze);

        if (!wall_front() && !get_cell(maze, get_cell_id_in_direction(robot->orientation, robot->position, maze, FRONT))->visited) {
            move_forward(robot, 1);
        } else if (!wall_left() && !get_cell(maze, get_cell_id_in_direction(robot->orientation, robot->position, maze, LEFT))->visited) {
            turn_left(robot);
            move_forward(robot, 1);
        } else if (!wall_right() && !get_cell(maze, get_cell_id_in_direction(robot->orientation, robot->position, maze, RIGHT))->visited) {
            turn_right(robot);
            move_forward(robot, 1);
        } else { // go back to predecessor cell

            int predecessor_cell_id = get_cell(maze, robot->position)->predecessor_cell_id;
            int orientation = get_orientation_to_neighbor_cell(robot->position, predecessor_cell_id);
            turn_to_orientation(robot, orientation);
            move_forward(robot, 1);
        }
    }
    while (robot->position != starting_position);
    
    // Turn robot such that it faces north
    turn_to_orientation(robot, NORTH);
}

int exploit_bfs(Robot* robot, Cell maze[MAZE_SIZE][MAZE_SIZE], int goals_cell_ids[4]) {
    node_t* head = NULL;
    int current_cell_id;
    Cell* current_cell;
    int neighbor_cell_id;
    Cell* neighbor_cell;
    int orientation;

    enqueue(&head, robot->position);

    while (1) {
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
                    return neighbor_cell_id;
                }
                enqueue(&head, neighbor_cell_id);
            }
        }
    }
}

void explore_and_exploit() {
    Robot robot;
    int num_path_elements;
    int goal_cell_id;
    init_robot(&robot);
    Cell maze[MAZE_SIZE][MAZE_SIZE];
    init_maze(maze);

    exploration_dfs(&robot, maze);
    unvisit_all_cells(maze);

    int goal_cells_ids[4] = {0};
    calc_goal_cells_ids(goal_cells_ids);
    goal_cell_id = exploit_bfs(&robot, maze, goal_cells_ids);

    int cell_sequence[MAZE_SIZE * MAZE_SIZE];
    num_path_elements = reconstruct_path_to_goal(goal_cell_id, maze, cell_sequence);

    Array orientation_sequence;
    initArray(&orientation_sequence, 4 * MAZE_SIZE);
    reconstruct_orientation_sequence_from_cell_sequence(maze, cell_sequence, num_path_elements, &orientation_sequence);
    
    generate_actions_from_orientation_sequence(&robot, orientation_sequence);
    freeArray(&orientation_sequence);
}

// returns updated cell_sequence
// cell_sequence needs size MAZE_SIZE * MAZE_SIZE and must be initialization with zeros: "int cell_sequence[MAZE_SIZE * MAZE_SIZE];"
// return number of elements in path (remaining array remains filled with zeros)
int reconstruct_path_to_goal(const int goal_cell_id, Cell maze[MAZE_SIZE][MAZE_SIZE], int cell_sequence[MAZE_SIZE * MAZE_SIZE]) {
    int cell_id = goal_cell_id;
    int num_path_elements = 0;
    Cell* cell = get_cell(maze, cell_id);
    int i;

    for (i = 0; i < MAZE_SIZE * MAZE_SIZE; ++i) {
        cell_sequence[i] = cell_id;
        ++num_path_elements;
        if (cell_id == 0) { // reached starting position
            break;
        }
        cell_id = cell->predecessor_cell_id;
        cell = get_cell(maze, cell_id);
    }
    reverse(cell_sequence, num_path_elements);
    return num_path_elements;
}

// returns updated cell_sequence
// cell_sequence needs size MAZE_SIZE * MAZE_SIZE and must be initialization with zeros: "int cell_sequence[MAZE_SIZE * MAZE_SIZE];"
// Array action sequence is modified in-place and it must have size "num_path_elements - 1"
void reconstruct_orientation_sequence_from_cell_sequence(Cell maze[MAZE_SIZE][MAZE_SIZE], int cell_sequence[MAZE_SIZE * MAZE_SIZE], const int num_path_elements, Array* orientation_sequence) {
    int i;
    int orientation;
    for (i = 0; i < num_path_elements - 1; ++i) {
        orientation = get_orientation_to_neighbor_cell(cell_sequence[i], cell_sequence[i + 1]);
        insertArray(orientation_sequence, orientation);
    }
}

void generate_actions_from_orientation_sequence(Robot* robot, Array orientation_sequence) {
    int i;
    for (i = 0; i < orientation_sequence.used; ++i) {
        turn_to_orientation(robot, orientation_sequence.array[i]);
        move_forward(robot, 1);
    }
}