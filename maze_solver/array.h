#pragma once

typedef struct {
    int *array;
    unsigned int used;
    unsigned int size;
} Array;

void initArray(Array *a, unsigned int initialSize);

void insertArray(Array *a, int element);

void freeArray(Array *a);