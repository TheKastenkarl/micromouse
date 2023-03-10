#ifndef QUEUE_H
#define	QUEUE_H

typedef struct node {
    int val;
    struct node* next;
} node_t;

void enqueue(node_t** head, int val);

int dequeue(node_t** head);

void print_list(node_t* head);

#endif /* QUEUE_H */