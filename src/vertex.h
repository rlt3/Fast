#ifndef FAST_VERTEX_H
#define FAST_VERTEX_H

#define Y_LENGTH  5.0f
#define X_LENGTH  7.0f

#define X 0
#define Y 1

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float** make_vertices_array(int length);

void    destroy_vertices_array(float **array, int length);

void    create_vertex(float vertex[], 
          float x, 
          float y, 
          float radius, 
          float base_angle,
          float vertex_angle);

float   distance(float a[], float b[]);

void    midpoint(float array[], float a[], float b[]);

float   triangle_area(float x[], float y[], float z[]);

#endif
