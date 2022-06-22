// Robot
// HYPERPARAMETER according to the laying MacroMouse prototype
#define ROBOT_DIMENSION_X 12
#define ROBOT_DIMENSION_Y 12

// Grid
// HYPERPARAMETER according to laying grid prototype
#define CELL_SIDE_COUNT 8
#define END_IDX_Y 4
#define END_IDX_X 4


enum Direction {
    LEFT = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 3
};

struct Node {
  int cell_x;
  int cell_y;

  struct Node* left;
  struct Node* up;
  struct Node* right;
  struct Node* down;

  // Maze solution
  int visited = 0;
  // reached from which previous node
  struct Node* reached_from;
};

void connect_cells(Node* first, Direction direction, Node* second);
// direction_second = (direction_first + 2) % 4
//                                          & 0b11


// Datastructures
#define SIZE 40

struct queue {
  struct Node* items[SIZE];
  int front;
  int rear;
};

struct queue* createQueue();
void enqueue(struct queue* q, struct Node*);
struct Node* dequeue(struct queue* q);
void display(struct queue* q);
int isEmpty(struct queue* q);
void printQueue(struct queue* q);