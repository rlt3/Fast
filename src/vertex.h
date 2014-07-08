#ifndef FAST_VERTEX_H
#define FAST_VERTEX_H

#include <math.h>

struct Vertex {
  float x;
  float y;
};

struct Vertex create_vertex(float x, float y, float radius, float angle);

#endif
