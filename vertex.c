#include "vertex.h"

/*
 * Point Vertex B towards Vertex A. B's angle is relative to A's, so we need to
 * keep the new vertex's angle the same as B's angle.
 */
struct Vertex 
vertex_point_to(struct Vertex a, struct Vertex b)
{
  struct Vertex p = _create_vertex(a.x, a.y, b.radius, (b.angle + a.angle));
  p.angle = b.angle;
  return p;
}

/*
 * Shift a vertex by its angle to a new location based on the amount given 
 */
struct Vertex 
vertex_shift(struct Vertex vertex, float amount)
{
  return _create_vertex(vertex.x, vertex.y, amount, vertex.angle);
}

struct Vertex
_create_vertex(float x, float y, float radius, float angle)
{
  return (struct Vertex) {
    .x = (x + (radius * sin(angle * (M_PI/180)))),
    .y = (y + (radius * cos(angle * (M_PI/180)))),
    .angle  = angle,
    .radius = radius
  };
}
