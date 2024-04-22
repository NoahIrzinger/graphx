#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graw.h"

#define ARRAY_SIZE 100

int main()
{
    // unsigned int x_values[] = {1, 5, 2, 3, 4};
    // unsigned int y_values[] = {1, 5, 2, 3, 4};
    // unsigned int values[] = {1, 2, 3, 4, 5};

    // int x_values[ARRAY_SIZE];
    // int y_values[ARRAY_SIZE];
    // int values[ARRAY_SIZE];

    // int max = 20;
    // int min = -20;

    // srand(time(NULL)); // Seed the random number generator

    // // Generate positive and negative random values for x_values, y_values, and values arrays
    // for (int i = 0; i < ARRAY_SIZE; i++)
    // {
    //     x_values[i] = (rand() % (max - min + 1)) + min; // Generates random integers between -100 and 100
    //     y_values[i] = (rand() % (max - min + 1)) + min; // Generates random integers between -100 and 100
    //     values[i] = (rand() % (max - min + 1)) + min;   // Generates random integers between -100 and 100
    // }

    unsigned int x_values[ARRAY_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
    unsigned int y_values[ARRAY_SIZE] = {0, 0, 1, 2, 2, 2, 2, 1, 0, -1, -3, -5, -6, -8, -9, -9, -9, -9, -8, -7, -5, -4, -2, 0, 0, 1, 2, 2, 2, 2, 1, 0, 0, -1, -2, -2, -2, -2, -1, 0, 1, 2, 4, 6, 7, 8, 9, 9, 9, 9, 8, 6, 4, 3, 1, 0, -1, -2, -2, -2, -2, -1, 0, 0, 1, 1, 2, 2, 2, 1, 0, 0, -2, -3, -5, -7, -8, -9, -9, -9, -9, -8, -7, -5, -3, -2, 0, 0, 1, 2, 2, 2, 2, 1, 0, 0, -1, -2, -2, -2};
    unsigned int values[ARRAY_SIZE] = {0, 0, 2, 6, 8, 10, 12, 7, 0, -9, -30, -55, -72, -104, -126, -135, -144, -153, -144, -133, -100, -84, -44, 0, 0, 25, 52, 54, 56, 58, 30, 0, 0, -33, -68, -70, -72, -74, -38, 0, 40, 82, 168, 258, 308, 360, 414, 423, 432, 441, 400, 306, 208, 159, 54, 0, -56, -114, -116, -118, -120, -61, 0, 0, 64, 65, 132, 134, 136, 69, 0, 0, -144, -219, -370, -525, -608, -693, -702, -711, -720, -648, -574, -415, -252, -170, 0, 0, 88, 178, 180, 182, 184, 93, 0, 0, -96, -194, -196, -198};

    unsigned int x_values_size = sizeof(x_values) / sizeof(x_values[0]);
    unsigned int y_values_size = sizeof(y_values) / sizeof(y_values[0]);
    unsigned int values_size = sizeof(values) / sizeof(values[0]);

    points_plot(x_values, y_values, values, values_size);

    return 0;
}