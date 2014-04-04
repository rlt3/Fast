#ifndef FAST_VERTEX_H
#define FAST_VERTEX_H

#include <math.h>

struct Vertex {
  float x;
  float y;
  int angle;
};

struct Vertex vertex_point_to(struct Vertex base, 
                              struct Vertex vertex, 
                              float         radius);

struct Vertex vertex_shift(struct Vertex vertex, float amount);
struct Vertex _create_vertex(float x, float y, float radius, float angle);

#endif
