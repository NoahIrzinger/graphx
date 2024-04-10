#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned int PIXEL_LENGTH = 12;

static unsigned int GRAPH_PIXELS_DEFAULT_WIDTH = 300;
static unsigned int GRAPH_PIXELS_DEFAULT_HEIGHT = 300;
static unsigned int GRAPH_TICK_PIXELS_DEFAULT_INTERVAL = 10;

struct Point
{
    int x_pos;
    int y_pos;
    size_t value;
};

void write_raster_to_ppm(char raster_data[], char output_file_path[], int graph_pixels_width, int graph_pixels_height)
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

void paint_raster(char *raster, int raster_row_length, int hex_values[], int graph_pixels_width, int graph_pixels_height)
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

    printf("%s\n", raster);
}

void generate_hexcodes(int *hexcodes, int raster_row_length, int graph_pixels_width, int graph_pixels_height, int graph_tick_pixels_interval, struct Point points[])
{
    int PLOT_Y_AXIS_OFFSET = 5;
    int PLOT_Y_AXIS_WIDTH = 1;
    int PLOT_X_AXIS_OFFSET = 5;
    int PLOT_X_AXIS_WIDTH = 1;

    for (int row_pos = 0; row_pos < graph_pixels_height; row_pos++)
    {
        for (int col_pos = 0; col_pos < graph_pixels_width; col_pos++)
        {
            int hex_index = row_pos * raster_row_length + col_pos;

            hexcodes[hex_index] = 0x00FF00;

            if (col_pos == PLOT_Y_AXIS_OFFSET)
            {
                hexcodes[hex_index] = 0x000000;
            }
            if (row_pos == graph_pixels_height - PLOT_X_AXIS_OFFSET)
            {
                hexcodes[hex_index] = 0x000000;
            }
            if (
                row_pos % graph_tick_pixels_interval == 0 && (col_pos == PLOT_Y_AXIS_OFFSET - 1 || (col_pos > PLOT_Y_AXIS_OFFSET && col_pos <= PLOT_Y_AXIS_OFFSET + 3)))
            {
                hexcodes[hex_index] = 0x000000;
            }
            if (
                col_pos % graph_tick_pixels_interval == 0 && (row_pos == (graph_pixels_height - PLOT_X_AXIS_OFFSET + 1) || (row_pos <= graph_pixels_height - PLOT_X_AXIS_OFFSET && row_pos > (graph_pixels_height - PLOT_X_AXIS_OFFSET - 3))))
            {
                hexcodes[hex_index] = 0x000000;
            }
        }
    }
}

void set_graph_dimensions(unsigned int *graph_pixels_width, unsigned int *graph_pixels_height, unsigned int *graph_tick_pixels_interval) {

    *graph_pixels_width = GRAPH_PIXELS_DEFAULT_WIDTH;
    *graph_pixels_height = GRAPH_PIXELS_DEFAULT_HEIGHT;
    *graph_tick_pixels_interval = GRAPH_TICK_PIXELS_DEFAULT_INTERVAL; 
}

int main()
{
    char path[] = "output/example.ppm";

    int colour_background = 0x00FF00;

    struct Point points[1];
    points[0].value = 1;
    points[0].x_pos = 1;
    points[0].y_pos = 1;

    unsigned int graph_pixels_width;
    unsigned int graph_pixels_height;
    unsigned int graph_tick_pixels_interval;

    set_graph_dimensions(&graph_pixels_width, &graph_pixels_height, &graph_tick_pixels_interval);

    int raster_row_length = (graph_pixels_width * PIXEL_LENGTH) + 1;   // the number of characters representing pixels in a row times 9 digits for the triplet and each of their white spaces
    int raster_length = (raster_row_length * graph_pixels_height) + 1; // the number of characters in total as a single line this is the length of all the pixels

    char raster[raster_length];
    memset(raster, 0, sizeof(raster));

    int hexcodes[raster_length];
    memset(hexcodes, 0, sizeof(hexcodes));

    generate_hexcodes(hexcodes, raster_row_length, graph_pixels_width, graph_pixels_height, graph_tick_pixels_interval, points);

    paint_raster(raster, raster_row_length, hexcodes, graph_pixels_width, graph_pixels_height);

    // printf("%s\n", raster_data);

    write_raster_to_ppm(raster, path, graph_pixels_width, graph_pixels_height);

    return 0;
}
