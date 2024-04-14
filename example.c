#include <stdio.h>

#include "graphx.h"

int main()
{
    unsigned int x_values[] = {1, 5, 2, 3, 4};
    unsigned int y_values[] = {1, 5, 2, 3, 4};
    unsigned int values[] = {1, 2, 3, 4, 5};

    unsigned int x_values_size = sizeof(x_values) / sizeof(x_values[0]);
    unsigned int y_values_size = sizeof(y_values) / sizeof(y_values[0]);
    unsigned int values_size = sizeof(values) / sizeof(values[0]);

    plot_points(x_values, x_values_size, y_values, y_values_size, values, values_size);

    return 0;
}