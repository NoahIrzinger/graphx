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
#define GRAPH_SIZE_SCALE 20
#define GRAPH_POINT_RADIUS_SIZE_PIXELS 2

struct Point
{
    int data_x_pos;
    int data_y_pos;
    unsigned int graph_x_pos;
    unsigned int graph_y_pos;
    size_t value;
};

void write_raster_to_ppm(char raster_data[], char output_file_path[], unsigned int graph_pixels_width, unsigned int graph_pixels_height)
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

void paint_raster(char *raster, unsigned int raster_row_length, int hex_values[], unsigned int graph_pixels_width, unsigned int graph_pixels_height)
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

void generate_hexcodes(int *hexcodes, unsigned int raster_row_length, unsigned int graph_pixels_width, unsigned int graph_pixels_height, struct Point points[], unsigned int points_length)
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
                int graph_x_pos = points[i].data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
                int graph_y_pos = graph_origin_y_pos - points[i].data_y_pos * GRAPH_SIZE_SCALE;
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
                // draw line graph
                if (i < points_length - 1)
                {
                    int next_graph_x_pos = points[i + 1].data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
                    int next_graph_y_pos = graph_origin_y_pos - points[i + 1].data_y_pos * GRAPH_SIZE_SCALE;

                    if (col_pos > graph_x_pos && col_pos < next_graph_x_pos)
                    {
                        double y_diff = next_graph_y_pos - graph_y_pos;
                        double x_diff = next_graph_x_pos - graph_x_pos;

                        if (x_diff != 0)
                        {
                            double points_slope = y_diff / x_diff;

                            double cur_y_diff = next_graph_y_pos - row_pos;
                            double cur_x_diff = next_graph_x_pos - col_pos;

                            if (cur_y_diff != 0)
                            {
                                double cur_slope = cur_y_diff / cur_x_diff;

                                if (cur_slope == points_slope)
                                {
                                    hexcodes[hex_index] = GRAPH_POINTS_COLOUR;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void calculate_graph_size(struct Point *points, unsigned int points_length, unsigned int *graph_pixels_width, unsigned int *graph_pixels_height)
{
    int max_distance = 0;
    struct Point furthest_point;

    for (int i = 0; i < points_length; i++)
    {
        int distance = sqrt(
            (double)points[i].data_x_pos * points[i].data_x_pos + (double)points[i].data_y_pos * points[i].data_y_pos);

        if (distance > max_distance)
        {
            max_distance = distance;
            furthest_point = points[i];
        }
    }

    *graph_pixels_width = abs(furthest_point.data_x_pos * 4 * GRAPH_SIZE_SCALE);
    *graph_pixels_height = abs(furthest_point.data_y_pos * 4 * GRAPH_SIZE_SCALE);
}

int validate_input_data(unsigned int x_values[], unsigned int x_values_size, unsigned int y_values[], unsigned int y_values_size, unsigned int values[], unsigned int values_size)
{
    if (x_values_size != y_values_size || x_values_size != values_size || y_values_size != values_size)
    {
        printf("the input arrays are not equal\n");
        return 1;
    }
    return 0;
}

int compare_points(const void *point_a, const void *point_b)
{
    return (((struct Point *)point_a)->data_x_pos - ((struct Point *)point_b)->data_y_pos);
}

void generate_points(struct Point *points, unsigned int number_of_points, unsigned int x_values[], unsigned int y_values[], unsigned int values[])
{
    for (int i = 0; i < number_of_points; i++)
    {
        struct Point p = {x_values[i], y_values[i], 0, 0, values[i]};
        points[i] = p;
    }
    qsort(points, number_of_points, sizeof(struct Point), compare_points);
}

int plot_points(unsigned int x_values[], unsigned int x_values_size, unsigned int y_values[], unsigned int y_values_size, unsigned int values[], unsigned int values_size)
{
    int err = validate_input_data(x_values, x_values_size, y_values, y_values_size, values, values_size);
    if (err != 0)
    {
        return err;
    }

    struct Point points[values_size];
    generate_points(points, values_size, x_values, y_values, values);

    unsigned int graph_pixels_width, graph_pixels_height;
    calculate_graph_size(points, values_size, &graph_pixels_width, &graph_pixels_height);

    unsigned int raster_row_length = graph_pixels_width * PIXEL_LENGTH + 1;    // the number of characters representing pixels in a row times 9 digits for the triplet and each of their white spaces
    unsigned long raster_length = raster_row_length * graph_pixels_height + 1; // the number of characters in total as a single line this is the length of all the pixels

    char *raster = (char *)malloc(raster_length * sizeof(char));
    int *hexcodes = (int *)malloc(raster_length * sizeof(int));

    generate_hexcodes(hexcodes, raster_row_length, graph_pixels_width, graph_pixels_height, points, values_size);

    paint_raster(raster, raster_row_length, hexcodes, graph_pixels_width, graph_pixels_height);

    write_raster_to_ppm(raster, OUTPUT_FILE_PATH, graph_pixels_width, graph_pixels_height);

    free(raster);
    free(hexcodes);

    return 0;
}
