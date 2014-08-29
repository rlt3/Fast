#ifndef FAST_POLYGON_H
#define FAST_POLYGON_H

#include <stdlib.h>
#include "vertex.h"

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

bool          point_inside_polygon(struct Vertex origin, struct Polygon p);
bool          triangle_intersects_polygon(struct Polygon triangle, 
                                          struct Polygon polygon);

bool          polygon_visible(struct Polygon p);
bool          below_screen(struct Polygon p);

void          deconstruct_polygon_array(struct Polygon *p[], int max);
void          deconstruct_polygon(struct Polygon *p);

#endif
