# graphx.c

This is a very simple graphing library written in C.

## Project Roadmap

1. Write graph visualization to PPM file format output.
1. Rendering engine for basic 2D cartisian coordinate system.
1. Define Point data type and associated containers.
1. In-memory data processing system to map Points to coordinate system in rendering engine.

## Architecture

The project is broken down in to the following components:

### Data Processor

Defines the schema and data structures for handling the visualization data including formatting and parsing.

### Graph Painter.

Handles generating pixel colour values and orienting them in the visualization.

In visual form, a two dimensional array of pixels with length and width will have each individual index map to an integer value in a "2D raster matrix"

This data is also represented in memory as a series of hexadecimal integers in an seperate one dimensional array as a "1D raster array"

Painting is done by specifying the desired pixel colour at a length and width index in the raster matrix and transfering that value to the same corresponding location in the raster array.

### File Writer

Writes raster data as a specified file type to a given path.

The writer supports the following formats:

1. PPM
