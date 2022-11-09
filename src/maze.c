#include <stdio.h>
#include "maze.h"
#include "API.h"

#if !SIMULATION
#include "serialComms.h"
#endif

/**
 * Initializes the maze:
 * - every cell has all 4 walls
 * - every cell is unvisited
 * - every cell has the no predecessor cell (equal to -1)
 * 
 * @param maze: Pointer to the uninitialized maze.
 */
void init_maze(Cell maze[MAZE_SIZE][MAZE_SIZE]) {
    int i, j;
    for (i = 0; i < MAZE_SIZE; ++i) {
        for (j = 0; j < MAZE_SIZE; ++j) {
            maze[i][j].walls = 0b1111;
            maze[i][j].visited = 0;
            maze[i][j].predecessor_cell_id = -1;
        }
    }
}

/**
 * Get the pointer of the cell with the given cell_id.
 * 
 * @param maze: Pointer to the maze.
 * @param cell_id: Cell id of the cell.
 * @return: Pointer to cell with "cell_id".
 */
Cell* get_cell(Cell maze[MAZE_SIZE][MAZE_SIZE], const int cell_id) {
    int row, col;
    row = cell_id / MAZE_SIZE;
    col = cell_id % MAZE_SIZE;
    return &maze[row][col];
}

/**
 * Returns positive value if the cell with "cell_id" has a wall in the given
 * orientation, else zero.
 * Uses stored maze and not the current sensor readings to determine answer.
 * 
 * @param maze: Pointer to the maze.
 * @param cell_id: Cell id of the cell.
 * @param orientation: Orientation (e.g. NORTH) of interest.
 * @return: Positive value if there is a wall in that orientation, else zero.
 */
int is_wall_in_orientation(Cell maze[MAZE_SIZE][MAZE_SIZE], const int cell_id, const int orientation) {
    Cell* cell = get_cell(maze, cell_id);
    return (cell->walls & orientation);
}

/**
 * Updates the walls of the stored maze based on sensor readings of robot.
 * Updates walls for the cell where the robot is currently in and also the walls
 * of the neighboring cells.
 * 
 * @param robot: Pointer to the robot.
 * @param maze: Pointer to the maze.
 */
void update_walls(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE]) {
    // WEST
    Cell* current_cell = get_cell(maze, robot->position);
    if (no_wall_west(robot)) {
        int cell_id_west;
        Cell* cell_west;

        current_cell->walls = current_cell->walls & 0b1110; // (~WEST & 0b1111)
        cell_id_west = get_cell_id_in_orientation(robot->position, maze, WEST);
        cell_west = get_cell(maze, cell_id_west);
        cell_west->walls = cell_west->walls & 0b1011; // cell in WEST has no wall in EAST
    }
    // NORTH
    if (no_wall_north(robot)) {
        int cell_id_north;
        Cell* cell_north;

        current_cell->walls = current_cell->walls & 0b1101;
        cell_id_north = get_cell_id_in_orientation(robot->position, maze, NORTH);
        cell_north = get_cell(maze, cell_id_north);
        cell_north->walls = cell_north->walls & 0b0111; // cell in NORTH has no wall in SOUTH
    }
    // EAST
    if (no_wall_east(robot)) {
        int cell_id_east;
        Cell* cell_east;

        current_cell->walls = current_cell->walls & 0b1011;
        cell_id_east = get_cell_id_in_orientation(robot->position, maze, EAST);
        cell_east = get_cell(maze, cell_id_east);
        cell_east->walls = cell_east->walls & 0b1110; // cell in EAST has no wall in WEST
    }
    // SOUTH
    if (no_wall_south(robot)) {
        int cell_id_south;
        Cell* cell_south;

        current_cell->walls = current_cell->walls & 0b0111;
        cell_id_south = get_cell_id_in_orientation(robot->position, maze, SOUTH);
        cell_south = get_cell(maze, cell_id_south);
        cell_south->walls = cell_south->walls & 0b1101; // cell in SOUTH has no wall in NORTH
    }
}

/**
 * Updates stored maze based on sensor readings of robot and the position of the robot:
 * - Updates wall
 * - Updates visited status of cell where the robot is currently in
 * - Updates predecessor cell id of cell where the robot is currently in if the value has not been set already
 * 
 * @param robot: Pointer to the robot.
 * @param maze: Pointer to the maze.
 * @param predecessor_cell_id: Cell id of predecessor cell of current cell.
 */
void update_cell(Robot* const robot, Cell maze[MAZE_SIZE][MAZE_SIZE], const int predecessor_cell_id) {
    Cell* cell = get_cell(maze, robot->position);
    cell->visited = 1;
    update_walls(robot, maze);
    // Set predecessor only if the cell not already has a predecessor
    if (cell->predecessor_cell_id == -1) {
        cell->predecessor_cell_id = predecessor_cell_id;
    }
}

/**
 * Unvisits all cells of the maze:
 * - Set visited status of all cells to 0
 * - Set predecessor cell id for all cells to -1
 * 
 * @param maze: Pointer to the maze.
 */
void unvisit_all_cells(Cell maze[MAZE_SIZE][MAZE_SIZE]){
    int i, j;
    for (i = 0; i < MAZE_SIZE; ++i) {
        for (j = 0; j < MAZE_SIZE; ++j) {
            maze[i][j].visited = 0;
            maze[i][j].predecessor_cell_id = -1;
        }
    }
}

/**
 * Calculates the cell id(s) of the goal position(s) of a maze.
 * The goal position(s) is/are always in the center of the maze.
 * - Case 1: uneven number of rows/columns -> 1 center cell (remaining array is filled with -1)
 * - Case 2: even number of rows/columns -> 4 center cells
 * 
 * @param goal_cells_ids: Empty array where the cell id(s) of the goal(s) are written back to.
 */
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

/**
 * Returns orientation from cell with cell_id to neighboring cell with neighboring_cell_id.
 * Example: Neighboring cell with cell_id=25 is north of cell with cell_id=9.
 *          Then NORTH is returned.
 * Only works for adjecent cells. If that is not fulfilled, return -1.
 * Does not check for borders, e.g. cell ids 9 and 10 might not be neighbors
 * if the size of the maze is 10. That is not detected.
 * 
 * @param cell_id: Cell id.
 * @param neighboring_cell_id: Cell id of neighboring cell.
 * @return: Orientation of neighboring cell w.r.t cell.
 */
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

/**
 * Gets cell which is in the provided orientation (e.g. NORTH) of the provided cell with "cell_id".
 * If the cell has no neighboring cell in that direction (due to the borders of the maze), -1 is returned.
 * 
 * @param cell_id: Cell id.
 * @param maze: Pointer to the maze.
 * @param orientation: Orientation of interest (e.g. NORTH)
 * @return: Cell id of neighboring cell in provided orientation if that cell exists.
 */
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
    } else { // should never be required for correct inputs
        return -1;
    }
}

/**
 * Gets cell which is in the provided direction (e.g. LEFT) of the provided cell with "cell_id".
 * To be able to convert the direction into a orientation (e.g. NORTH), the orientation in the cell with
 * "cell_id" has to be provided.
 * It is not detected if the cell has no neighboring cell in that direction (due to the borders of the maze).
 * In that case, the returned value makes no sense.
 * 
 * @param current_orientation: Orientation in cell with "cell_id" (e.g. NORTH)
 * @param cell_id: Cell id.
 * @param maze: Pointer to the maze.
 * @param direction: Direction of interest (e.g. FRONT)
 * @return: Cell id of neighboring cell in provided direction
 */
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
    } else { // // should never be required for correct inputs as all cases are covered
        return -1;
    }
}


/**
 * Logs for every cell of the maze the position of the walls (in integer format).
 * Works both in simulation and on actual robot (UART).
 * West (0b0001), North (0b0010), East (0b0100), South (0b1000), combine them with bitwise OR.
 * 
 * @param maze: Pointer to the maze.
 */
#if SIMULATION
void log_cell_walls(Cell maze[MAZE_SIZE][MAZE_SIZE]){
    API_log("MAZE:");
    char str[10];
    for (int i = 0; i < MAZE_SIZE; ++i) {
        for (int j = 0; j < MAZE_SIZE; ++j) {
            API_log("Cell ID (row, column):");
            sprintf(str, "%d", i);
            API_log(str);
            sprintf(str, "%d", j);
            API_log(str);
            API_log("Cell walls:");
            sprintf(str, "%d", maze[i][j].walls);
            API_log(str);
            API_log("-------------------");
        }
    }
}
#else
void log_cell_walls(Cell maze[MAZE_SIZE][MAZE_SIZE]){
    sendUART1("MAZE:", 1);
    char str[10];
    for (int i = 0; i < MAZE_SIZE; ++i) {
        for (int j = 0; j < MAZE_SIZE; ++j) {
            sendUART1("Cell ID (row, column):", 1);
            sprintf(str, "%d", i);
            sendUART1(str, 1);
            sprintf(str, "%d", j);
            sendUART1(str, 1);
            sendUART1("Cell walls:", 1);
            sprintf(str, "%d", maze[i][j].walls);
            sendUART1(str, 1);
            sendUART1("-------------------", 1);
        }
    }
}
#endif

/**
 * Visualizes the specified cell sequence in the simulation in blue.
 * In each cell, the distance to the last cell of the cell sequence is written.
 * It removes all old texts and colors.
 * 
 * @param cell_sequence: Cell sequence which should be visualized.
 */
#if SIMULATION && VISUALIZATION
void visualize_cell_sequence(Array* cell_sequence) {
    int i, row, col, remaining_distance;
    API_clearAllText();
    API_clearAllColor();

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
#if SIMULATION && VISUALIZATION
void mark_cell(const int cell_id) {
    int row = cell_id / MAZE_SIZE;
    int col = cell_id % MAZE_SIZE;
    char str[5];
    sprintf(str, "%d", cell_id);
    API_setText(col, row, str);
    API_setColor(col, row, 'G');
}
#endif