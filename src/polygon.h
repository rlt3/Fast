#ifndef FAST_POLYGON_H
#define FAST_POLYGON_H

#define PLAYER_ANGLES    3
#define ASTEROID_ANGLES  4
#define STAR_ANGLES      4

#include <stdbool.h>
#include "vertex.h"

/*
 * A polygon holds is just n points derived from some angle. A polygon here has
 * a center and base angle. Its vertices are derived from that center and angle.
 *
 * The radius is how 'wide' the polygon is displayed. Sides is what kind of 
 * polygon it might be -- triangle, quad . . .
 */

struct Polygon {
  /* center of the polygon */
  float x;
  float y;

  /* the angles of the point from the center */
  int  *angles;

  /* vertices from the center point for drawing + bounding */
  float **vertices;

  /* structure: where it's pointing and what kind of polygon */
  float radius;
  int   angle;
  int   sides;

};

struct Polygon* construct_player();
struct Polygon* construct_asteroid();
struct Polygon* construct_star();
void            update_vertices(struct Polygon *p);

bool          polygon_visible(struct Polygon p);
bool          below_screen(struct Polygon p);
bool          above_screen(struct Polygon p);

float         polygon_area(struct Polygon p);
bool          point_inside_polygon(float origin[], struct Polygon p);
bool          triangle_intersects_polygon(struct Polygon triangle, 
                struct Polygon polygon);

void          deconstruct_polygon_array(struct Polygon *p[], int max);
void          deconstruct_polygon(struct Polygon *p);

#endif
