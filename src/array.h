#ifndef ARRAY_H
#define	ARRAY_H

typedef struct {
    int *array;
    unsigned int used;
    unsigned int size;
} Array;

void init_array(Array* a, unsigned int initial_size);

void insert_array(Array* a, int element);

void free_array(Array* a);

void reverse_array(Array* a);

#endif /* ARRAY_H */