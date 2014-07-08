#include "polygon.h"

struct Vertex
vertex_shift(struct Polygon p, float amount)
{
  return create_vertex(p.center.x, p.center.y, amount, p.angle);
}

struct Vertex
vertex_from_angle(struct Polygon p, float angle)
{
  return create_vertex(p.center.x, p.center.y, p.radius, (p.angle + angle));
}
