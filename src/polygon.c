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
