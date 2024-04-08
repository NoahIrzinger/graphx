#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned int PIXEL_LENGTH = 12;

void write_to_ppm_file(char path[], int pixels_width, int pixels_height, char raster_data[])
{

    char MAGIC_NUMBER[] = "P3";
    int MAXVAL = 255;

    printf("%s\n", path);

    FILE *f;
    f = fopen(path, "w");

    fprintf(f, "%s\n%d %d\n%d\n", MAGIC_NUMBER, pixels_width, pixels_height, MAXVAL);

    fprintf(f, "%s", raster_data);

    fclose(f);
}

void paint_raster_data(char *raster_data, int row_length, int hex_values[], int pixels_width, int pixels_height)
{

    for (int i = 0; i < pixels_height; i++)
    {
        char row[row_length];
        memset(row, 0, sizeof(row));

        for (int j = 0; j < pixels_width; j++)
        {
            char next_pixel[PIXEL_LENGTH];
            memset(next_pixel, 0, sizeof(next_pixel));

            sprintf(next_pixel, " %u %u %u", (hex_values[i + j] >> 16) & 0xFF, (hex_values[i + j] >> 8) & 0xFF, hex_values[i + j] & 0xFF);
            strcat(row, next_pixel);
        }

        strcat(row, "\n");
        strcat(raster_data, row);
    }

    strcat(raster_data, "\0");

    printf("%s\n", raster_data);
}

void fill_with_colour(int hex_code, int *hex_values, int raster_length)
{
    for (int i = 0; i < raster_length; i++)
    {
        hex_values[i] = hex_code;
    }
}

int main()
{
    char path[] = "output/example.ppm";

    int hex_code = 0xFF0000;

    int pixels_width = 250;
    int pixels_height = 400;

    int row_length = (pixels_width * PIXEL_LENGTH) + 1;   // the number of pixels in a row times 9 digits for the triplet and each of their white spaces
    int raster_length = (row_length * pixels_height) + 1; // as a single line this is the length of all the pixels

    char raster_data[raster_length];
    memset(raster_data, 0, sizeof(raster_data));

    int hex_values[raster_length];
    memset(hex_values, 0, sizeof(hex_values));

    fill_with_colour(hex_code, hex_values, raster_length);

    paint_raster_data(raster_data, row_length, hex_values, pixels_width, pixels_height);

    // printf("%s\n", raster_data);

    write_to_ppm_file(path, pixels_width, pixels_height, raster_data);

    return 0;
}
