from enum import Enum
from multiprocessing.dummy import current_process
from tkinter import N

# Robot
# HYPERPARAMETER according to the laying MacroMouse prototype
robot_dimention_x = 12
robot_dimention_y = 12
# center position
pos_x: float = robot_dimention_x / 2
pos_y: float = robot_dimention_y / 2

# Grid
# HYPERPARAMETER according to laying grid prototype
cell_side_count = 8
# square array representing grid
grid = []

start = 0,0
end = cell_side_count/2 - 1, cell_side_count/2 - 1

class Direction(Enum):
    LEFT = 0
    UP = 1
    RIGHT = 2
    DOWN = 3


# single cell is 18x18cm
class Node:
    # center of cell
    x: int
    y: int
    # pointers
    # connection to neighboring cells
    left = None
    right = None
    up = None
    down = None

    # Maze solution
    visited = False
    # reached from which previous node
    reached_from = None

    def __init__(self,cell_x,cell_y):
        self.x = (cell_x)*18 + 9
        self.y = (cell_y)*18 + 9

    def cell_connect(connected_node,direction: Direction):
        if direction == Direction.LEFT:
            left = connected_node
        elif direction == Direction.UP:
            up = connected_node
        elif direction == Direction.RIGHT:
            right = connected_node
        elif direction == Direction.DOWN:
            down = connected_node

def init():
    global grid
    global cell_side_count
    grid = [[ Node(x,y) for x in range(cell_side_count)] for y in range(cell_side_count)]

def explore():

    return

def getGoalPath():
    queue = []
    start_node = grid[start[0]][start[1]]
    end_node = grid[end[0]][end[1]]

    # enqueue
    queue.append(start_node)
    start_node.visited = True

    while not queue == []:
        # dequeue
        node = queue.pop(0)
        possible_next = [node.left, node.up, node.right, node.left]
        for next in possible_next:
            if next != None:
                next.visited = True
                next.reached_from = node

                if next == end_node:
                    backtrack(next)
                    # empty queue
                    return
                elif next.visited == False:
                    queue.append(next)


def backtrack(node:Node):
    directions = []
    current_node = node
    while current_node.reached_from != None:
        prev_node = current_node.reached_from
        direction = None
        if current_node == prev_node.left:
            direction = Direction.LEFT
        if current_node == prev_node.right:
            direction = Direction.RIGHT
        if current_node == prev_node.up:
            direction = Direction.UP
        if current_node == prev_node.down:
            direction = Direction.DOWN
        # add direction to first position
        directions.insert(0,direction)

        current_node = prev_node
    return