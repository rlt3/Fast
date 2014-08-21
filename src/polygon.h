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
  struct Vertex  center;
  struct Vertex *vertices;
  float          radius;
  int            sides;
};

void          update_vertices(struct Polygon *p);
struct Vertex vertex_from_angle(struct Polygon p, float angle);

float         distance(struct Vertex a, struct Vertex b);
float         triangle_area(struct Vertex x, struct Vertex y, struct Vertex z);
float         polygon_area(struct Polygon p);

#endif
