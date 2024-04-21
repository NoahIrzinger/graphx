#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "graphx.h"

#define GRAPH_BACKGROUND_COLOUR 0x00FF00
#define GRAPH_AXIS_COLOUR 0x000000
#define GRAPH_POINTS_COLOUR 0x000000
#define OUTPUT_FILE_PATH "output/plot_points.ppm"
#define PIXEL_LENGTH 12
#define GRAPH_SIZE_SCALE 3
#define GRAPH_POINT_RADIUS_SIZE_PIXELS 1

struct Point
{
    int data_x_pos;
    int data_y_pos;
    int graph_x_pos;
    int graph_y_pos;
    int value;
};

void raster_write_to_ppm(char raster_data[], char output_file_path[], unsigned int graph_pixels_width, unsigned int graph_pixels_height)
{

    char MAGIC_NUMBER[] = "P3";
    int MAXVAL = 255;

    printf("%s\n", output_file_path);

    FILE *f;
    f = fopen(output_file_path, "w");

    fprintf(f, "%s\n%d %d\n%d\n", MAGIC_NUMBER, graph_pixels_width, graph_pixels_height, MAXVAL);

    fprintf(f, "%s", raster_data);

    fclose(f);
}

void raster_paint(char *raster, unsigned int raster_row_length, int hex_values[], unsigned int graph_pixels_width, unsigned int graph_pixels_height)
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

            sprintf(next_pixel, " %u %u %u", (hex_values[hex_index] >> 16) & 0xFF, (hex_values[hex_index] >> 8) & 0xFF, hex_values[hex_index] & 0xFF);
            strcat(row, next_pixel);
        }

        strcat(row, "\n");
        strcat(raster, row);
    }

    strcat(raster, "\0");
}

void points_compute_bresenham(struct Point *points_array, unsigned int points_length, struct Point *bresenham_points_array, unsigned int *bresenham_points_array_size, unsigned int graph_pixels_width, unsigned int graph_pixels_height)
{
    int graph_origin_y_pos = graph_pixels_height / 2;
    int graph_origin_x_pos = graph_pixels_width / 2;

    unsigned int j = 0;

    for (unsigned int i = 0; i < points_length - 1; i++)
    {
        struct Point cur_point = points_array[i];
        struct Point next_point = points_array[i + 1];

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
            bresenham_points_array[j] = p;

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
    *bresenham_points_array_size = j;
}

void raster_generate_hexcodes(int *hexcodes, unsigned int raster_row_length, unsigned int graph_pixels_width, unsigned int graph_pixels_height, struct Point points_array[], unsigned int points_length, struct Point *bresenham_points_array, unsigned int *bresenham_points_array_size)
{
    int graph_origin_y_pos = graph_pixels_height / 2;
    int graph_origin_x_pos = graph_pixels_width / 2;

    for (int row_pos = 0; row_pos < graph_pixels_height; row_pos++)
    {
        for (int col_pos = 0; col_pos < graph_pixels_width; col_pos++)
        {
            int hex_index = row_pos * raster_row_length + col_pos;

            // fill background
            hexcodes[hex_index] = GRAPH_BACKGROUND_COLOUR;
            // x,y
            if (col_pos == graph_origin_y_pos)
            {
                hexcodes[hex_index] = GRAPH_AXIS_COLOUR;
            }
            if (row_pos == graph_origin_x_pos)
            {
                hexcodes[hex_index] = GRAPH_AXIS_COLOUR;
            }
            // ticks on the axis
            if (row_pos % GRAPH_SIZE_SCALE == 0 && (col_pos == graph_origin_y_pos - 1 || col_pos == graph_origin_y_pos + 1))
            {
                hexcodes[hex_index] = GRAPH_AXIS_COLOUR;
            }
            if (col_pos % GRAPH_SIZE_SCALE == 0 && (row_pos == graph_origin_x_pos - 1 || row_pos == graph_origin_x_pos + 1))
            {
                hexcodes[hex_index] = GRAPH_AXIS_COLOUR;
            }
            // plot points
            for (int i = 0; i < points_length; i++)
            {
                int graph_x_pos = points_array[i].data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
                int graph_y_pos = graph_origin_y_pos - points_array[i].data_y_pos * GRAPH_SIZE_SCALE;
                if (col_pos == graph_x_pos && row_pos == graph_y_pos)
                {
                    hexcodes[hex_index] = GRAPH_POINTS_COLOUR;
                }
                // scale up points size on graph
                if (
                    col_pos <= graph_x_pos + GRAPH_POINT_RADIUS_SIZE_PIXELS && col_pos >= graph_x_pos - GRAPH_POINT_RADIUS_SIZE_PIXELS && row_pos <= graph_y_pos + GRAPH_POINT_RADIUS_SIZE_PIXELS && row_pos >= graph_y_pos - GRAPH_POINT_RADIUS_SIZE_PIXELS)
                {
                    hexcodes[hex_index] = GRAPH_POINTS_COLOUR;
                }
            }
            // use Bresenham's algorithm to draw line between points
            for (int i = 0; i < *bresenham_points_array_size; i++)
            {
                if (col_pos == bresenham_points_array[i].graph_x_pos && row_pos == bresenham_points_array[i].graph_y_pos)
                {
                    hexcodes[hex_index] = GRAPH_POINTS_COLOUR;
                }
            }
        }
    }
}

void graph_calculate_dimensions(struct Point *points_array, unsigned int points_length, unsigned int *graph_pixels_width, unsigned int *graph_pixels_height)
{
    int max_distance = 0;
    struct Point furthest_point;

    for (int i = 0; i < points_length; i++)
    {
        int distance = sqrt(
            (double)points_array[i].data_x_pos * points_array[i].data_x_pos + (double)points_array[i].data_y_pos * points_array[i].data_y_pos);

        if (distance > max_distance)
        {
            max_distance = distance;
            furthest_point = points_array[i];
        }
    }

    *graph_pixels_width = abs(furthest_point.data_x_pos * 4 * GRAPH_SIZE_SCALE);
    *graph_pixels_height = abs(furthest_point.data_y_pos * 4 * GRAPH_SIZE_SCALE);
}

int points_qsort_compare(const void *point_a, const void *point_b)
{
    if (((struct Point *)point_a)->data_x_pos < ((struct Point *)point_b)->data_x_pos)
    {
        return -1;
    }
    if (((struct Point *)point_a)->data_x_pos > ((struct Point *)point_b)->data_x_pos)
    {
        return 1;
    }
    return 0;
}

void points_generate_array(struct Point *points_array, unsigned int number_of_points, int x_values[], int y_values[], int values[])
{
    for (unsigned int i = 0; i < number_of_points; i++)
    {
        struct Point p = {x_values[i], y_values[i], 0, 0, values[i]};
        points_array[i] = p;
    }
    qsort(points_array, number_of_points, sizeof(struct Point), points_qsort_compare);
}

int points_plot(int x_values[], int y_values[], int values[], unsigned int points_size)
{
    struct Point points_array[points_size];
    points_generate_array(points_array, points_size, x_values, y_values, values);
    printf("generated points_array\n");

    unsigned int graph_pixels_width, graph_pixels_height;
    graph_calculate_dimensions(points_array, points_size, &graph_pixels_width, &graph_pixels_height);
    printf("calculated graph size\n");

    unsigned int raster_row_length = graph_pixels_width * PIXEL_LENGTH + 1;    // the number of characters representing pixels in a row times 9 digits for the triplet and each of their white spaces
    unsigned long raster_length = raster_row_length * graph_pixels_height + 1; // the number of characters in total as a single line this is the length of all the pixels

    char *raster; 
    raster = (char *)malloc(raster_length * (sizeof *raster));

    int *hexcodes;
    hexcodes = (int *)malloc(raster_length * (sizeof *hexcodes));

    unsigned int bresenham_points_array_size = raster_length; // at first set equal to the raster length and realloc when we know the number of pixel to draw lines
    struct Point *bresenham_points_array;
    bresenham_points_array = (struct Point *)malloc(bresenham_points_array_size * (sizeof *bresenham_points_array)); // estimate the max size by the longer size of the length or width multiplied by the number of points

    points_compute_bresenham(points_array, points_size, bresenham_points_array, &bresenham_points_array_size, graph_pixels_width, graph_pixels_height);
    printf("computed points using bresenham algorithm\n");

    raster_generate_hexcodes(hexcodes, raster_row_length, graph_pixels_width, graph_pixels_height, points_array, points_size, bresenham_points_array, &bresenham_points_array_size);
    printf("generated hex codes\n");

    raster_paint(raster, raster_row_length, hexcodes, graph_pixels_width, graph_pixels_height);
    printf("painted raster\n");

    raster_write_to_ppm(raster, OUTPUT_FILE_PATH, graph_pixels_width, graph_pixels_height);
    printf("raster data written to file\n");

    free(raster);
    free(hexcodes);
    free(bresenham_points_array);

    return 0;
}
