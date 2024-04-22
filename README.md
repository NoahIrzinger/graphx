# graw.c

Graphical Drawing Library

## Overview

This library generates an output file as a rendered image of two-dimensional input data in Cartisian coordinates.

## Interface

#### function
### points_plot

    int points_plot(int x_values[], int y_values[], int values[], unsigned int points_size);

Parses a pair of coordinates (X,Y) and their corresponding value into a line plot.

The function will write a **P3** variant [Portable Pixel Map](https://netpbm.sourceforge.net/doc/ppm.html) file to at the path: `outputs/plot_points.ppm`.

## Testing

Executing the main function in `example.c` with generate a test image.

The `graw.py` file uses [ctypes](https://docs.python.org/3/library/ctypes.html) allow usage of `graw.c` interface as a foriegn function library to extend its usage into Python.

#### On a Linux machine run:

`gcc -std=c99 -fPIC -shared -o graw.so graw.c` - to build the shared library

 `python graw.py` - to run generate the test visualization from the Python environment