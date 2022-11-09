#ifndef SETTINGS_H
#define	SETTINGS_H

/**
 * Important: With the microchip compiler you have to allocate heap memory in the project properties
 *            (https://microchipdeveloper.com/mplabx:creating-a-heap). Use for example 4096 bytes.
 */

// Number of columns (= number of rows) of maze
#define MAZE_SIZE 6

// Define if simulation or real robot is used
#define SIMULATION 1

// Define if in simulation visualization should be used
#define VISUALIZATION 0

#endif	/* SETTINGS_H */