#include "polygon.h"

#define RIGHT         1
#define LEFT         -1
#define NONE          0

#define BOUND_TOP     0
#define BOUND_RIGHT   1
#define BOUND_BOTTOM  2
#define BOUND_LEFT    3

bool 
polygon_point_inside(struct Vertex point, 
                     struct Vertex vertices[], 
                     int           length)
{
  int previous_side = NONE;
  int current_side;
  struct Vertex a, b, affine_segment, affine_point;
  
  int n;
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

/*
 * top, right, bottom, left | 0, 1, 2, 3
 */
void 
polygon_bounds(struct Polygon p, float *bounds)
{
  /* set the bounds to zero so we can test properly */
  memset(bounds, 0, sizeof(bounds));

  int i;
  for (i = 0; i < p.points; i++) {
    if (p.vertices[i].x < bounds[BOUND_LEFT]) {
      bounds[BOUND_LEFT] = p.vertices[i].x;
    }

    if (p.vertices[i].x > bounds[BOUND_RIGHT]) {
      bounds[BOUND_RIGHT] = p.vertices[i].x;
    }

    if (p.vertices[i].y < bounds[BOUND_TOP]) {
      bounds[BOUND_TOP] = p.vertices[i].y;
    }

    if (p.vertices[i].y > bounds[BOUND_BOTTOM]) {
      bounds[BOUND_BOTTOM] = p.vertices[i].y;
    }
  }
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
