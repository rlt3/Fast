#include <stdio.h>
#include "polygon.h"

void
update_vertices(struct Polygon *p) 
//update_vertices_array(struct Polygon *p, float array[][2]) 
{
  int i;
  for (i = 0; i < p->sides; i++) {
    //p->vertices[i] = vertex_from_angle(*p, p->vertices[i].angle);
    create_vertex(p->vertices[i], p->x, p->y, p->radius, 
        p->angle, p->angles[i]);
  }
}

struct Polygon *
construct_player()
{
  struct Polygon *p = (struct Polygon*) malloc(sizeof(struct Polygon));

  if (p == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  *p = (struct Polygon) {
    .x      = 0.0f,
    .y      = -5.0f,
    .radius = 0.75,
    .angle  = 90,
    .sides  = PLAYER_ANGLES
  };

  p->angles    = (int*) malloc(sizeof(int) * PLAYER_ANGLES);

  if (p->angles == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  p->angles[0] =    0;
  p->angles[1] = -135;
  p->angles[2] =  135;

  p->vertices = make_vertices_array(p->sides);

  int i;
  for (i = 0; i < p->sides; i++) {
    create_vertex(p->vertices[i], p->x, p->y, p->radius, 
        p->angle, p->angles[i]);
  }

  return p;
}

struct Polygon *
construct_asteroid() 
{
  struct Polygon *p = (struct Polygon*) malloc(sizeof(struct Polygon));

  if (p == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  *p = (struct Polygon) {
    /* 
     * Generate a random number between 0.0f and 1.0f, multiply it
     * by 10 to get a range from 0.0f to 10.0f and then subtract 5.0f
     * to get a final range of -5.0f to 5.0f -- the width of the screen
     */
    .x      = ((drand48() * 10.0f) - 5.0f),
    .y      = 5.0f,

    .radius = 0.5,
    .angle  = rand() % 91,
    .sides  = ASTEROID_ANGLES
  };

  p->angles = (int*) malloc(sizeof(int) * ASTEROID_ANGLES);

  if (p->angles == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  p->vertices = make_vertices_array(p->sides);

  /* handle any polygon by splitting 360 degrees among the sides */
  int angle_portion = 360 / p->sides;

  /* make our asteroids random by generating an angle for each side */
  int i;
  for (i = 0; i < p->sides; i++) {
    p->angles[i] = (angle_portion * i) + rand() % 91;

    create_vertex(p->vertices[i], p->x, p->y, p->radius, 
        p->angle, p->angles[i]);
  }

  return p;
}


///* update vertices in polygon */
//struct Vertex
//vertex_from_angle(struct Polygon p, float angle)
//{
//  return create_vertex(p.center.x, p.center.y, p.radius, p.center.angle, angle);
//}
//
///* the distance formula */
//float
//distance(struct Vertex a, struct Vertex b)
//{
//  return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
//}

///* the midpoint formula */
//struct Vertex
//midpoint(struct Vertex a, struct Vertex b)
//{
//  return (struct Vertex) {
//    .x = ((a.x + b.x) / 2),
//    .y = ((a.y + b.y) / 2),
//  };
//}
//
//float
//triangle_area(struct Vertex x, struct Vertex y, struct Vertex z)
//{
//  float a = distance(x, y);
//  float b = distance(y, z);
//  float c = distance(z, x);
//
//  float s = (a + b + c) / 2;
//
//  return sqrt(s * (s - a) * (s - b) * (s - c));
//}
//
///*
// * Every polygon is comprised of triangles. So, split the polygon into many
// * triangles and just count the areas of each triangle to get the area of
// * that polygon.
// */
//float
//polygon_area(struct Polygon p)
//{
//  float total_area    = 0.0f;
//  int   num_triangles = p.sides - 2;
//  int   loops         = 0;
//  int   i;
//
//  for (i = 0; loops < num_triangles; i = i + 2) {
//    total_area += triangle_area(p.vertices[i], 
//                                p.vertices[(i + 1) % p.sides], 
//                                p.vertices[(i + 2) % p.sides]);
//    loops++;
//  }
//
//  return total_area;
//}
//
///*
// * Given a point, create triangles using that point and each side of the polygon
// * as the base. If the total area of these triangles equal the area of the 
// * polygon then that point is inside the polygon.
// */
//bool
//point_inside_polygon(struct Vertex origin, struct Polygon p)
//{
//  float total_area    = 0.0f;
//  float area          = polygon_area(p);
//  
//  int   num_triangles = p.sides;
//  int   i;
//
//  /* accepted variance for floating point comparison */
//  float epsilon       = 0.00001;
//
//  /* total the area of the sub-triangles created by the origin point */
//  for (i = 0; i < num_triangles; i++) {
//    total_area += triangle_area(origin, 
//                                p.vertices[i],
//                                p.vertices[(i + 1) % p.sides]);
//  }
//
//  return (fabs(total_area - area) < epsilon);
//}
//
///*
// * Test the three main points plus the midpoints of the triangle for collision 
// */
//bool
//triangle_intersects_polygon(struct Polygon triangle, struct Polygon polygon)
//{
//  int num_points = triangle.sides * 2;
//
//  struct Vertex points[num_points];
//
//  /* Gather all of our test points at once */
//  int i, x;
//  for (i = 0; i < num_points; i = i + 2) {
//    /* Our vertices are half the number of points, so divide and subtract */
//    x = i - (i / 2);
//
//    points[i]     = triangle.vertices[x];
//    points[i + 1] = midpoint(triangle.vertices[x], 
//                             triangle.vertices[(x + 1) % triangle.sides]);
//  }
//
//  bool intersects = false;
//
//  for (i = 0; i < (triangle.sides * 2); i++) {
//    if (point_inside_polygon(points[i], polygon)) {
//      intersects = true;
//      break;
//    }
//  }
//
//  return intersects;
//}
//
//bool
//polygon_visible(struct Polygon p)
//{
//  bool visible = false;
//  int i;
//  for (i = 0; i < p.sides; i++) {
//    if (vertex_visible(p.vertices[i])) {
//      visible = true;
//      break;
//    }
//  }
//  return visible;
//}
//
//bool
//below_screen(struct Polygon p)
//{
//  return (!polygon_visible(p) && p.center.y < -Y_LENGTH);
//}

void
deconstruct_polygon_array(struct Polygon *p[], int max)
{
  int i;
  for (i = 0; i < max; i++) {
    if (p[i] == NULL) { continue; }
    deconstruct_polygon(p[i]);
    p[i] = NULL;
  }
}

void
deconstruct_polygon(struct Polygon *p)
{
  destroy_vertices_array(p->vertices, p->sides);
  free(p->angles);
  free(p);
}
