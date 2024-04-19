#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graphx.h"

#define ARRAY_SIZE 10

int main()
{
    // unsigned int x_values[] = {1, 5, 2, 3, 4};
    // unsigned int y_values[] = {1, 5, 2, 3, 4};
    // unsigned int values[] = {1, 2, 3, 4, 5};

    int x_values[ARRAY_SIZE];
    int y_values[ARRAY_SIZE];
    int values[ARRAY_SIZE];

    int max = 100;
    int min = -100;

    srand(time(NULL)); // Seed the random number generator

    // Generate positive and negative random values for x_values, y_values, and values arrays
    for (int i = 0; i < ARRAY_SIZE; i++) {
        x_values[i] = (rand() % (max - min + 1)) + min; // Generates random integers between -100 and 100
        y_values[i] = (rand() % (max - min + 1)) + min; // Generates random integers between -100 and 100
        values[i] = (rand() % (max - min + 1)) + min;   // Generates random integers between -100 and 100
    }

    unsigned int x_values_size = sizeof(x_values) / sizeof(x_values[0]);
    unsigned int y_values_size = sizeof(y_values) / sizeof(y_values[0]);
    unsigned int values_size =  sizeof(values) / sizeof(values[0]);

    plot_points(x_values, x_values_size, y_values, y_values_size, values, values_size);

    return 0;
}