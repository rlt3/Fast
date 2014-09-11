#ifndef FAST_POLYGON_H
#define FAST_POLYGON_H

#define PLAYER_ANGLES    3
#define ASTEROID_ANGLES  4
#define STAR_ANGLES      4

#include "vertex.h"

/*
 * All vertices have an angle that is relative to the center.
 * This is so the shape of the polygon can remain the same as
 * it rotates and moves.
 */

/*
 * A polygon holds its angles.
 *
 * A bounding box is its points derived from its current location (for collision
 * with other objects). Its vertices are derived from point (0, 0) and are
 * used for drawing the polygon.
 */

struct Polygon {
  /* center of the polygon */
  float x;
  float y;

  /* the angles of the point from the center */
  int  *angles;

  /* vertices from the center point for drawing + bounding */
  float **vertices;
  float **bounds;

  /* structure: where it's pointing and what kind of polygon */
  float radius;
  int   angle;
  int   sides;

};

struct Polygon* construct_player();
struct Polygon* construct_asteroid();
void            update_vertices(struct Polygon *p);

//struct Vertex vertex_from_angle(struct Polygon p, float angle);
//
//float         distance(struct Vertex a, struct Vertex b);
//float         triangle_area(struct Vertex x, struct Vertex y, struct Vertex z);
//float         polygon_area(struct Polygon p);
//
//bool          point_inside_polygon(struct Vertex origin, struct Polygon p);
//bool          triangle_intersects_polygon(struct Polygon triangle, 
//                                          struct Polygon polygon);
//
//bool          polygon_visible(struct Polygon p);
//bool          below_screen(struct Polygon p);

void          deconstruct_polygon_array(struct Polygon *p[], int max);
void          deconstruct_polygon(struct Polygon *p);

#endif
