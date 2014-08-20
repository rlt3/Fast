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
  return create_vertex(p.center.x, p.center.y, p.radius, (p.center.angle + angle));
}

/* the distance formula */
float
distance(struct Vertex a, struct Vertex b)
{
//  float dist = sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
//  printf("(%f, %f) -> (%f, %f) => %f\n", a.x, a.y, b.x, b.y, dist);
//  return dist;
  return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
}

float
triangle_area(struct Polygon polygon)
{
//  struct Vertex vertices[3];
//
//  int i;
//  for (i = 0; i < 3; i++) {
//    vertices[i] = vertex_from_angle(polygon, polygon.angles[i]);
//  }
//
//  float a = distance(vertices[0], vertices[1]);
//  float b = distance(vertices[1], vertices[2]);
//  float c = distance(vertices[2], vertices[0]);
//
//  float s = (a + b + c) / 2;
//
//  return sqrt(s * (s - a) * (s - b) * (s - c));
    return 2.5;
}
