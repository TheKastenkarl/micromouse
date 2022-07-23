#include <stdlib.h>
#include "array.h"

// Source: https://stackoverflow.com/questions/3536153/c-dynamically-growing-array

void init_array(Array* a, unsigned int initialSize) {
    a->array = malloc(initialSize * sizeof(int));
    a->used = 0;
    a->size = initialSize;
}

void insert_array(Array* a, int element) {
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size 
    if (a->used == a->size) {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(int));
    }
    a->array[a->used++] = element;
}

void free_array(Array* a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

// Function to reverse elements of an array
void reverse_array(Array* a)
{
    for (int low = 0, high = a->used - 1; low < high; low++, high--) {
        int temp = a->array[low];
        a->array[low] = a->array[high];
        a->array[high] = temp;
    }
}