#include "vertex.h"

float** 
make_vertices_array(int length)
{
  float **array = (float**) malloc(sizeof(float*) * length);

  if (array == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  int i;
  for (i = 0; i < length; i++) {
    /* we're only doing two dimensions */
    array[i] = (float*) malloc(sizeof(float) * 2);

    if (array[i] == NULL) {
      fprintf(stderr, "%s\n", "Out of memory.");
      exit(1);
    }
  }

  return array;
}

void
destroy_vertices_array(float **array, int length)
{
  int i;
  for (i = 0; i < length; i++) {
    free(array[i]);
  }

  free(array);
}

void
create_vertex(float vertex[], 
    float x, 
    float y, 
    float radius, 
    float base_angle,
    float vertex_angle)
{
  vertex[X] = (x + (radius * cos((vertex_angle + base_angle) * (M_PI/180))));
  vertex[Y] = (y + (radius * sin((vertex_angle + base_angle) * (M_PI/180))));
}

/* the distance formula */
float
distance(float a[], float b[])
{
  return sqrt(pow((b[X] - a[X]), 2) + pow((b[Y] - a[Y]), 2));
}

///* the midpoint formula */
//struct Vertex
//midpoint(struct Vertex a, struct Vertex b)
//{
//  return (struct Vertex) {
//    .x = ((a.x + b.x) / 2),
//    .y = ((a.y + b.y) / 2),
//  };
//}

//struct Vertex
//create_vertex(float x, float y, float radius, float offset, float angle)
//{
//  return (struct Vertex) {
//    .x = (x + (radius * cos((angle + offset) * (M_PI/180)))),
//    .y = (y + (radius * sin((angle + offset) * (M_PI/180)))),
//    .angle = angle
//  };
//}
//
//bool
//vertex_visible(struct Vertex vertex)
//{
//  return (vertex.y > -Y_LENGTH &&
//          vertex.y <  Y_LENGTH &&
//          vertex.x > -X_LENGTH &&
//          vertex.x <  X_LENGTH);
//}
