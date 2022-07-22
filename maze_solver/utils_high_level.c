#include "utils_high_level.h"

int is_present(const int element, int* arr, const int arr_length) {
    int i;
    for (i = 0; i < arr_length; ++i) {
        if (arr[i] == element) {
            return 1;
        }
    }
    return 0;
}

// Function to reverse elements of an array
void reverse(int arr[], int n)
{
    for (int low = 0, high = n - 1; low < high; low++, high--) {
        int temp = arr[low];
        arr[low] = arr[high];
        arr[high] = temp;
    }
}