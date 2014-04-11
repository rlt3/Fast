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
  struct Vertex  center;   
  struct Vertex *vertices; 
  int            points;   

  float          bounds[4];
};

bool          polygon_point_inside(struct Vertex point, 
                                   struct Vertex vertices[], 
                                   int           length);

void          polygon_bounds(struct Polygon p, float *bounds);


struct Vertex _v_sub(struct Vertex a, struct Vertex b);
int           _x_product(struct Vertex a, struct Vertex b);
int           _get_side(struct Vertex a, struct Vertex b);

#endif
