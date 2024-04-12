#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static unsigned int PIXEL_LENGTH = 12;
static unsigned int GRAPH_SIZE_SCALE = 20;
static unsigned int POINT_RADIUS_SIZE_PIXELS = 5;

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

    // printf("%s\n", raster);
}

void generate_hexcodes(int *hexcodes, unsigned int raster_row_length, unsigned int graph_pixels_width, unsigned int graph_pixels_height, struct Point points[], unsigned int points_length)
{
    int PLOT_Y_AXIS_OFFSET = 5;
    int PLOT_Y_AXIS_WIDTH = 1;
    int PLOT_X_AXIS_OFFSET = 5;
    int PLOT_X_AXIS_WIDTH = 1;

    int graph_origin_y_pos = graph_pixels_height / 2;
    int graph_origin_x_pos = graph_pixels_width / 2;

    for (int row_pos = 0; row_pos < graph_pixels_height; row_pos++)
    {
        for (int col_pos = 0; col_pos < graph_pixels_width; col_pos++)
        {
            int hex_index = row_pos * raster_row_length + col_pos;

            // fill background
            hexcodes[hex_index] = 0x00FF00;
            // x,y
            if (col_pos == graph_origin_y_pos)
            {
                hexcodes[hex_index] = 0x000000;
            }
            if (row_pos == graph_origin_x_pos)
            {
                hexcodes[hex_index] = 0x000000;
            }
            // ticks on the axis
            if (row_pos % GRAPH_SIZE_SCALE == 0 && (col_pos == graph_origin_y_pos - 1 || col_pos == graph_origin_y_pos + 1))
            {
                hexcodes[hex_index] = 0x000000;
            }
            if (col_pos % GRAPH_SIZE_SCALE == 0 && (row_pos == graph_origin_x_pos - 1 || row_pos == graph_origin_x_pos + 1))
            {
                hexcodes[hex_index] = 0x000000;
            }
            // plot points
            for (int i = 0; i < points_length; i++)
            {
                int graph_x_pos = points[i].data_x_pos * GRAPH_SIZE_SCALE + graph_origin_x_pos;
                int graph_y_pos = graph_origin_y_pos - points[i].data_y_pos * GRAPH_SIZE_SCALE;
                if (col_pos == graph_x_pos && row_pos == graph_y_pos)
                {
                    hexcodes[hex_index] = 0x000000;
                }
                // scale up points size on graph
                if (
                    col_pos <= graph_x_pos + POINT_RADIUS_SIZE_PIXELS && col_pos >= graph_x_pos - POINT_RADIUS_SIZE_PIXELS && row_pos <= graph_y_pos + POINT_RADIUS_SIZE_PIXELS && row_pos >= graph_y_pos - POINT_RADIUS_SIZE_PIXELS)
                {
                    hexcodes[hex_index] = 0x000000;
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

int main()
{
    char path[] = "output/example.ppm";

    int colour_background = 0x00FF00;

    struct Point points[6] = {
        {-2, 2, 0, 0, 1},
        {3, 3, 0, 0, 1},
        {-5, -5, 0, 0, 1},
        {9, -9, 0, 0, 1},
        {4, 4, 0, 0, 1},
        {6, 6, 0, 0, 1}};

    unsigned int graph_pixels_width, graph_pixels_height;

    unsigned int points_length = sizeof(points) / sizeof(points[0]);
    calculate_graph_size(points, points_length, &graph_pixels_width, &graph_pixels_height);

    unsigned int raster_row_length = graph_pixels_width * PIXEL_LENGTH + 1;    // the number of characters representing pixels in a row times 9 digits for the triplet and each of their white spaces
    unsigned long raster_length = raster_row_length * graph_pixels_height + 1; // the number of characters in total as a single line this is the length of all the pixels

    // char raster[raster_length];
    // memset(raster, 0, sizeof(raster));
    char *raster = (char *)malloc(raster_length * sizeof(char));

    // int hexcodes[raster_length];
    // memset(hexcodes, 0, sizeof(hexcodes));
    int *hexcodes = (int *)calloc(raster_length, sizeof(int));

    generate_hexcodes(hexcodes, raster_row_length, graph_pixels_width, graph_pixels_height, points, points_length);

    paint_raster(raster, raster_row_length, hexcodes, graph_pixels_width, graph_pixels_height);

    // printf("%s\n", raster_data);

    write_raster_to_ppm(raster, path, graph_pixels_width, graph_pixels_height);

    return 0;
}
