#ifndef FAST_POLYGON_H
#define FAST_POLYGON_H

#include <stdbool.h>
#include <string.h>
#include "vertex.h"

/*
 * All vertices have an angle that is relative to the center.
 * This is so the shape of the polygon can remain the same as
 * it rotates and moves.
 */

struct Polygon {
  struct Vertex center;
  float        *angles;

  int           angle;
  float         radius;
};

struct Vertex vertex_shift(struct Polygon p, float amount);
struct Vertex vertex_from_angle(struct Polygon p, float angle);

#endif
