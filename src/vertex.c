#include "vertex.h"

struct Vertex
create_vertex(float x, float y, float radius, float angle)
{
  return (struct Vertex) {
    .x = (x + (radius * cos(angle * (M_PI/180)))),
    .y = (y + (radius * sin(angle * (M_PI/180)))),
  };
}
