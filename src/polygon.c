#include <stdio.h>
#include "polygon.h"

void
update_vertices(struct Polygon *p) 
{
  int i;
  for (i = 0; i < p->sides; i++) {
    p->vertices[i] = vertex_from_angle(*p, p->vertices[i].angle);
  }
}

/* update vertices in polygon */
struct Vertex
vertex_from_angle(struct Polygon p, float angle)
{
  return create_vertex(p.center.x, p.center.y, p.radius, p.center.angle, angle);
}

/* the distance formula */
float
distance(struct Vertex a, struct Vertex b)
{
  return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
}

/* the midpoint formula */
struct Vertex
midpoint(struct Vertex a, struct Vertex b)
{
  return (struct Vertex) {
    .x = ((a.x + b.x) / 2),
    .y = ((a.y + b.y) / 2),
  };
}

float
triangle_area(struct Vertex x, struct Vertex y, struct Vertex z)
{
  float a = distance(x, y);
  float b = distance(y, z);
  float c = distance(z, x);

  float s = (a + b + c) / 2;

  return sqrt(s * (s - a) * (s - b) * (s - c));
}

/*
 * Every polygon is comprised of triangles. So, split the polygon into many
 * triangles and just count the areas of each triangle to get the area of
 * that polygon.
 */
float
polygon_area(struct Polygon p)
{
  float total_area    = 0.0f;
  int   num_triangles = p.sides - 2;
  int   loops         = 0;
  int   i;

  for (i = 0; loops < num_triangles; i = i + 2) {
    total_area += triangle_area(p.vertices[i], 
                                p.vertices[(i + 1) % p.sides], 
                                p.vertices[(i + 2) % p.sides]);
    loops++;
  }

  return total_area;
}

/*
 * Given a point, create triangles using that point and each side of the polygon
 * as the base. If the total area of these triangles equal the area of the 
 * polygon then that point is inside the polygon.
 */
bool
point_inside_polygon(struct Vertex origin, struct Polygon p)
{
  float total_area    = 0.0f;
  float area          = polygon_area(p);
  
  int   num_triangles = p.sides;
  int   i;

  /* accepted variance for floating point comparison */
  float epsilon       = 0.00001;

  /* total the area of the sub-triangles created by the origin point */
  for (i = 0; i < num_triangles; i++) {
    total_area += triangle_area(origin, 
                                p.vertices[i],
                                p.vertices[(i + 1) % p.sides]);
  }

  return (fabs(total_area - area) < epsilon);
}

/*
 * Test the three main points plus the midpoints of the triangle for collision 
 */
bool
triangle_intersects_polygon(struct Polygon triangle, struct Polygon polygon)
{
  int num_points = triangle.sides * 2;

  struct Vertex points[num_points];

  /* Gather all of our test points at once */
  int i, x;
  for (i = 0; i < num_points; i = i + 2) {
    /* Our vertices are half the number of points, so divide and subtract */
    x = i - (i / 2);

    points[i]     = triangle.vertices[x];
    points[i + 1] = midpoint(triangle.vertices[x], 
                             triangle.vertices[(x + 1) % triangle.sides]);
  }

  bool intersects = false;

  for (i = 0; i < (triangle.sides * 2); i++) {
    if (point_inside_polygon(points[i], polygon)) {
      intersects = true;
      break;
    }
  }

  return intersects;
}
