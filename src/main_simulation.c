#include "API.h"
#include "maze_solver.h"
#include "wall_follow.h"

#if SIMULATION
int main(int argc, char* argv[]) {
    explore_and_exploit();
    //wall_follow();
    return 0;
}
#endif