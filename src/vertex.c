#include "vertex.h"

struct Vertex
vertex_shift(struct Vertex vertex, float amount)
{
  return create_vertex(vertex.x, vertex.y, amount, 0, vertex.angle);
}

struct Vertex
create_vertex(float x, float y, float radius, float offset, float angle)
{
  return (struct Vertex) {
    .x = (x + (radius * cos((angle + offset) * (M_PI/180)))),
    .y = (y + (radius * sin((angle + offset) * (M_PI/180)))),
    .angle = angle
  };
}

bool
vertex_visible(struct Vertex vertex)
{
  return (vertex.y > -Y_LENGTH &&
          vertex.y <  Y_LENGTH &&
          vertex.x > -X_LENGTH &&
          vertex.x <  X_LENGTH);
}
