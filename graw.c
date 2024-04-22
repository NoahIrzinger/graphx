#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "graw.h"

#define GRAPH_BACKGROUND_COLOUR 0x00FF00
#define GRAPH_AXIS_COLOUR 0x000000
#define GRAPH_POINTS_COLOUR 0x000000
#define OUTPUT_FILE_PATH "points_plot.ppm"
#define PIXEL_LENGTH 12
#define GRAPH_SIZE_SCALE 3
#define GRAPH_POINT_RADIUS_SIZE_PIXELS 1
#define GRAPH_SIZE_X_DIMENSION_MIN_PIXELS 250
#define GRAPH_SIZE_Y_DIMENSION_MIN_PIXELS 250

struct Point
{
    int data_x_pos;
    int data_y_pos;
    int graph_x_pos;
    int graph_y_pos;
    int value;
};

void graph_calculate_dimensions(struct Point *points_array_ptr, unsigned int points_array_size, unsigned int *graph_pixels_width_ptr, unsigned int *graph_pixels_height_ptr)
{
    int max_distance = 0;
    struct Point point_furthest;

    for (int i = 0; i < points_array_size; i++)
    {
        int distance = sqrt(
            (double)points_array_ptr[i].data_x_pos * points_array_ptr[i].data_x_pos + (double)points_array_ptr[i].data_y_pos * points_array_ptr[i].data_y_pos);

        if (distance > max_distance)
        {
            max_distance = distance;
            point_furthest = points_array_ptr[i];
        }
    }

    *graph_pixels_width_ptr = abs(point_furthest.data_x_pos * 4 * GRAPH_SIZE_SCALE) < GRAPH_SIZE_X_DIMENSION_MIN_PIXELS ? GRAPH_SIZE_X_DIMENSION_MIN_PIXELS : abs(point_furthest.data_x_pos * 4 * GRAPH_SIZE_SCALE);
    *graph_pixels_height_ptr = abs(point_furthest.data_y_pos * 4 * GRAPH_SIZE_SCALE) < GRAPH_SIZE_Y_DIMENSION_MIN_PIXELS ? GRAPH_SIZE_Y_DIMENSION_MIN_PIXELS : abs(point_furthest.data_y_pos * 4 * GRAPH_SIZE_SCALE);
}

void raster_generate_hexcodes(int *hexcodes_array_ptr, unsigned int raster_row_length, unsigned int graph_pixels_width, unsigned int graph_pixels_height, struct Point *points_array_ptr, unsigned int points_array_size, struct Point *bresenham_points_array_ptr, unsigned int *bresenham_points_array_size)
{
    int graph_origin_y_pos = graph_pixels_height / 2;
    int graph_origin_x_pos = graph_pixels_width / 2;

    for (int row_pos = 0; row_pos < graph_pixels_height; row_pos++)
    {
        for (int col_pos = 0; col_pos < graph_pixels_width; col_pos++)
        {
            int hex_index = row_pos * raster_row_length + col_pos;

            // fill background
            hexcodes_array_ptr[hex_index] = GRAPH_BACKGROUND_COLOUR;
            // draw x and y lines
            if (col_pos == graph_origin_y_pos)
            {
                hexcodes_array_ptr[hex_index] = GRAPH_AXIS_COLOUR;
            }
            if (row_pos == graph_origin_x_pos)
            {
                hexcodes_array_ptr[hex_index] = GRAPH_AXIS_COLOUR;
            }
            // ticks on the axis
            if (row_pos % GRAPH_SIZE_SCALE == 0 && (col_pos == graph_origin_y_pos - 1 || col_pos == graph_origin_y_pos + 1))
            {
                hexcodes_array_ptr[hex_index] = GRAPH_AXIS_COLOUR;
            }
            if (col_pos % GRAPH_SIZE_SCALE == 0 && (row_pos == graph_origin_x_pos - 1 || row_pos == graph_origin_x_pos + 1))
            {
                hexcodes_array_ptr[hex_index] = GRAPH_AXIS_COLOUR;
            }
            // plot points
            for (int i = 0; i < points_array_size; i++)
            {
                int graph_x_pos = points_array_ptr[i].data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
                int graph_y_pos = graph_origin_y_pos - points_array_ptr[i].data_y_pos * GRAPH_SIZE_SCALE;
                if (col_pos == graph_x_pos && row_pos == graph_y_pos)
                {
                    hexcodes_array_ptr[hex_index] = GRAPH_POINTS_COLOUR;
                }
                // scale up points size on graph
                if (
                    col_pos <= graph_x_pos + GRAPH_POINT_RADIUS_SIZE_PIXELS && col_pos >= graph_x_pos - GRAPH_POINT_RADIUS_SIZE_PIXELS && row_pos <= graph_y_pos + GRAPH_POINT_RADIUS_SIZE_PIXELS && row_pos >= graph_y_pos - GRAPH_POINT_RADIUS_SIZE_PIXELS)
                {
                    hexcodes_array_ptr[hex_index] = GRAPH_POINTS_COLOUR;
                }
            }
            // use Bresenham's algorithm to draw line between points
            for (int i = 0; i < *bresenham_points_array_size; i++)
            {
                if (col_pos == bresenham_points_array_ptr[i].graph_x_pos && row_pos == bresenham_points_array_ptr[i].graph_y_pos)
                {
                    hexcodes_array_ptr[hex_index] = GRAPH_POINTS_COLOUR;
                }
            }
        }
    }
}

void raster_paint(char *raster_array_ptr, unsigned int raster_row_length, int *hexcodes_array_ptr, unsigned int graph_pixels_width, unsigned int graph_pixels_height)
{
    for (int i = 0; i < graph_pixels_height; i++)
    {
        char row[raster_row_length];
        memset(row, 0, sizeof(row));

        for (int j = 0; j < graph_pixels_width; j++)
        {
            int hex_index = i * raster_row_length + j;

            char next_pixel[PIXEL_LENGTH];
            memset(next_pixel, 0, sizeof(next_pixel));

            sprintf(next_pixel, " %u %u %u", (hexcodes_array_ptr[hex_index] >> 16) & 0xFF, (hexcodes_array_ptr[hex_index] >> 8) & 0xFF, hexcodes_array_ptr[hex_index] & 0xFF);
            strcat(row, next_pixel);
        }

        strcat(row, "\n");
        strcat(raster_array_ptr, row);
    }

    strcat(raster_array_ptr, "\0");
}

void raster_write_to_ppm(FILE *output_file_ptr, char *raster_array_ptr, unsigned int graph_pixels_width, unsigned int graph_pixels_height)
{
    char PPM_MAGIC_NUMBER[] = "P3";
    int PPM_MAXVAL = 255;

    fprintf(output_file_ptr, "%s\n%d %d\n%d\n", PPM_MAGIC_NUMBER, graph_pixels_width, graph_pixels_height, PPM_MAXVAL);

    fprintf(output_file_ptr, "%s", raster_array_ptr);

    fclose(output_file_ptr);
}

int points_qsort_compare(const void *point_a_ptr, const void *point_b_ptr)
{
    if (((struct Point *)point_a_ptr)->data_x_pos < ((struct Point *)point_b_ptr)->data_x_pos)
    {
        return -1;
    }
    if (((struct Point *)point_a_ptr)->data_x_pos > ((struct Point *)point_b_ptr)->data_x_pos)
    {
        return 1;
    }
    return 0;
}

void points_generate_array(struct Point *points_array_ptr, unsigned int points_array_size, int *x_values_array_ptr, int *y_values_array_ptr, int *values_array_ptr)
{
    for (unsigned int i = 0; i < points_array_size; i++)
    {
        struct Point p = {x_values_array_ptr[i], y_values_array_ptr[i], 0, 0, values_array_ptr[i]};
        points_array_ptr[i] = p;
    }
    qsort(points_array_ptr, points_array_size, sizeof(struct Point), points_qsort_compare);
}

void points_compute_bresenham(struct Point *points_array_ptr, unsigned int points_array_size, struct Point *bresenham_points_array_ptr, unsigned int *bresenham_points_array_size_ptr, unsigned int graph_pixels_width, unsigned int graph_pixels_height)
{
    int graph_origin_y_pos = graph_pixels_height / 2;
    int graph_origin_x_pos = graph_pixels_width / 2;

    unsigned int j = 0;

    for (unsigned int i = 0; i < points_array_size - 1; i++)
    {
        struct Point cur_point = points_array_ptr[i];
        struct Point next_point = points_array_ptr[i + 1];

        int cur_graph_x_pos = cur_point.data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
        int cur_graph_y_pos = graph_origin_y_pos - cur_point.data_y_pos * GRAPH_SIZE_SCALE;

        int next_graph_x_pos = next_point.data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
        int next_graph_y_pos = graph_origin_y_pos - next_point.data_y_pos * GRAPH_SIZE_SCALE;

        // compute line between these point's graph positions
        int dx = abs(next_graph_x_pos - cur_graph_x_pos);
        int dy = -abs(next_graph_y_pos - cur_graph_y_pos);

        int sx = next_graph_x_pos > cur_graph_x_pos ? 1 : -1;
        int sy = next_graph_y_pos > cur_graph_y_pos ? 1 : -1;

        int err = dx + dy;
        int e2;

        while (1)
        {
            struct Point p = {0, 0, cur_graph_x_pos, cur_graph_y_pos, 0};
            bresenham_points_array_ptr[j] = p;

            if (cur_graph_x_pos == next_graph_x_pos && cur_graph_y_pos == next_graph_y_pos)
                break;

            e2 = 2 * err;
            if (e2 >= dy)
            {
                err += dy;
                cur_graph_x_pos += sx;
            }
            if (e2 <= dx)
            {
                err += dx;
                cur_graph_y_pos += sy;
            }
            j++;
        }
    }
    // shorten bresenham_points_array_size
    *bresenham_points_array_size_ptr = j;
}

int points_plot(int *x_values_array_ptr, int *y_values_array_ptr, int *values_array_ptr, unsigned int points_array_size)
{
    FILE *output_file_p = fopen(OUTPUT_FILE_PATH, "w");
    if (output_file_p == NULL)
    {
        printf("Could not open file: %s\n", OUTPUT_FILE_PATH);
        return 1;
    }

    struct Point points_array[points_array_size];
    points_generate_array(points_array, points_array_size, x_values_array_ptr, y_values_array_ptr, values_array_ptr);
    printf("generated points_array\n");

    unsigned int graph_pixels_width, graph_pixels_height;
    graph_calculate_dimensions(points_array, points_array_size, &graph_pixels_width, &graph_pixels_height);
    printf("calculated graph size\n");

    unsigned int raster_row_length = graph_pixels_width * PIXEL_LENGTH + 1;          // the number of characters representing pixels in a row times 9 digits for the triplet and each of their white spaces
    unsigned long raster_total_length = raster_row_length * graph_pixels_height + 1; // the number of characters in total as a single line this is the length of all the pixels

    char *raster_array;
    raster_array = (char *)malloc(raster_total_length * (sizeof *raster_array));

    int *hexcodes_array;
    hexcodes_array = (int *)malloc(raster_total_length * (sizeof *hexcodes_array));

    unsigned int bresenham_points_array_size = raster_total_length; // at first set equal to the raster_array length and realloc when we know the number of pixel to draw lines
    struct Point *bresenham_points_array;
    bresenham_points_array = (struct Point *)malloc(bresenham_points_array_size * (sizeof *bresenham_points_array)); // estimate the max size by the longer size of the length or width multiplied by the number of points

    points_compute_bresenham(points_array, points_array_size, bresenham_points_array, &bresenham_points_array_size, graph_pixels_width, graph_pixels_height);
    printf("computed points using bresenham algorithm\n");

    raster_generate_hexcodes(hexcodes_array, raster_row_length, graph_pixels_width, graph_pixels_height, points_array, points_array_size, bresenham_points_array, &bresenham_points_array_size);
    printf("generated hex codes\n");

    raster_paint(raster_array, raster_row_length, hexcodes_array, graph_pixels_width, graph_pixels_height);
    printf("painted raster\n");

    raster_write_to_ppm(output_file_p, raster_array, graph_pixels_width, graph_pixels_height);
    printf("raster data written to file: %s\n", OUTPUT_FILE_PATH);

    free(raster_array);
    free(hexcodes_array);
    free(bresenham_points_array);

    return 0;
}