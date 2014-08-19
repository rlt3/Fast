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

bool 
polygon_point_inside(struct Vertex point, 
                     /*struct Vertex vertices[], */
                     struct Polygon polygon,
                     int           length)
{
  int previous_side = NONE;
  int current_side;
  int n;

  struct Vertex vertices[length];

  /* construct the vertices here */
  for (n = 0; n < length; n++) {
    vertices[n] = vertex_from_angle(polygon, polygon.angles[n]);
  }

  struct Vertex a, b, affine_segment, affine_point;
  
  for (n = 0; n < length; n++) {
    a = vertices[n];
    b = vertices[(n + 1) % length];

    affine_segment = _v_sub(b, a);
    affine_point   = _v_sub(point, b);
    current_side   = _get_side(affine_segment, affine_point);

    if (current_side == NONE) {
      /* outside or over an edge */
      return false;
    } else if (previous_side == NONE) { 
      /* First Segment */
      previous_side = current_side;
    } else if (previous_side != current_side) {
      return false;
    }
  }

  return true;
}

struct Vertex 
_v_sub(struct Vertex a, struct Vertex b)
{
  return (struct Vertex){(a.x - b.x), (a.y - b.y)};
}

int 
_x_product(struct Vertex a, struct Vertex b)
{
  return ((a.x * b.y) - (a.y * b.x));
}

int 
_get_side(struct Vertex a, struct Vertex b) 
{
  int x = _x_product(a, b);

  if (x < 0)
    return LEFT;
  else if (x > 0)
    return RIGHT;

  return NONE;
}

