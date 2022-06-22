// BFS algorithm in C

#include <stdio.h>
#include <stdlib.h>

#include "maze_solver.h"

// center position of robot
float pos_x;
float pos_y;


void init(){
  pos_x = ROBOT_DIMENSION_X / 2 ;
  pos_y = ROBOT_DIMENSION_Y / 2 ;
}

struct Node* createNode(int);

struct Graph {
  // square array representing grid
  struct Node* grid[CELL_SIDE_COUNT][CELL_SIDE_COUNT];

  int numVertices;
};

// BFS algorithm
void bfs(struct Graph* graph) {
  struct queue* q = createQueue();


  // start point
  graph->grid[0][0]->visited = 1;
  enqueue(q, graph->grid[0][0]);

  struct Node* end = graph->grid[END_IDX_Y][END_IDX_X];

  while (!isEmpty(q)) {
    // printQueue(q);
    struct Node* currentVertex = dequeue(q);
    // printf("Visited %d\n", currentVertex);

    // check the 4 directions
    struct Node* possibleNext[4] = {
      currentVertex->left,
      currentVertex->right,
      currentVertex->up,
      currentVertex->down
    };
    int i;
    for(i=0;i<4;i++){
      if (possibleNext[i] == NULL)
        continue;
      if (possibleNext[i]->visited == 0) {
        possibleNext[i]->visited = 1;
        enqueue(q, possibleNext[i]);
      }
    }


  }
}

// Creating a Node
struct Node* createNode(int v) {
  struct Node* newNode = malloc(sizeof(struct Node));
  newNode->vertex = v;
  newNode->next = NULL;
  return newNode;
}

// Creating a graph
struct Graph* createGraph(int vertices) {
  struct Graph* graph = malloc(sizeof(struct Graph));
  graph->numVertices = vertices;

  graph->adjLists = malloc(vertices * sizeof(struct Node*));
  graph->visited = malloc(vertices * sizeof(int));

  int i;
  for (i = 0; i < vertices; i++) {
    graph->adjLists[i] = NULL;
    graph->visited[i] = 0;
  }

  return graph;
}

// Add edge
void addEdge(struct Graph* graph, int src, int dest) {
  // Add edge from src to dest
  struct Node* newNode = createNode(dest);
  newNode->next = graph->adjLists[src];
  graph->adjLists[src] = newNode;

  // Add edge from dest to src
  newNode = createNode(src);
  newNode->next = graph->adjLists[dest];
  graph->adjLists[dest] = newNode;
}

// Create a queue
struct queue* createQueue() {
  struct queue* q = malloc(sizeof(struct queue));
  q->front = -1;
  q->rear = -1;
  return q;
}

// Check if the queue is empty
int isEmpty(struct queue* q) {
  if (q->rear == -1)
    return 1;
  else
    return 0;
}

// Adding elements into queue
void enqueue(struct queue* q, int value) {
  if (q->rear == SIZE - 1)
    printf("\nQueue is Full!!");
  else {
    if (q->front == -1)
      q->front = 0;
    q->rear++;
    q->items[q->rear] = value;
  }
}

// Removing elements from queue
int dequeue(struct queue* q) {
  int item;
  if (isEmpty(q)) {
    printf("Queue is empty");
    item = -1;
  } else {
    item = q->items[q->front];
    q->front++;
    if (q->front > q->rear) {
      printf("Resetting queue ");
      q->front = q->rear = -1;
    }
  }
  return item;
}

// Print the queue
void printQueue(struct queue* q) {
  int i = q->front;

  if (isEmpty(q)) {
    printf("Queue is empty");
  } else {
    printf("\nQueue contains \n");
    for (i = q->front; i < q->rear + 1; i++) {
      printf("%d ", q->items[i]);
    }
  }
}

int main() {
  struct Graph* graph = createGraph(6);
  addEdge(graph, 0, 1);
  addEdge(graph, 0, 2);
  addEdge(graph, 1, 2);
  addEdge(graph, 1, 4);
  addEdge(graph, 1, 3);
  addEdge(graph, 2, 4);
  addEdge(graph, 3, 4);

  bfs(graph, 0);

  return 0;
}