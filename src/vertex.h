#ifndef FAST_VERTEX_H
#define FAST_VERTEX_H

#define Y_LENGTH  4.0f
#define X_LENGTH  7.0f

#include <stdbool.h>
#include <math.h>

struct Vertex {
  float x;
  float y;
  int   angle;
};

struct Vertex vertex_shift(struct Vertex vertex, float amount);
struct Vertex create_vertex(float x, float y, float radius, 
                            float offset, float angle);
bool          vertex_visible(struct Vertex vertex);

#endif
