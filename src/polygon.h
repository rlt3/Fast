#ifndef FAST_POLYGON_H
#define FAST_POLYGON_H

#include "vertex.h"

#define RIGHT         1
#define LEFT         -1
#define NONE          0

/*
 * All vertices have an angle that is relative to the center.
 * This is so the shape of the polygon can remain the same as
 * it rotates and moves.
 */

struct Polygon {
  struct Vertex center;
  float        *angles;
  float         radius;
  int           angle;
};

struct Vertex vertex_shift(struct Polygon p, float amount);
struct Vertex vertex_from_angle(struct Polygon p, float angle);

bool          polygon_point_inside(struct Vertex point, 
                                   struct Polygon polygon,
                                   int length);

struct Vertex _v_sub(struct Vertex a, struct Vertex b);
int           _x_product(struct Vertex a, struct Vertex b);
int           _get_side(struct Vertex a, struct Vertex b);

#endif
