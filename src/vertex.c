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

/* Create a vertex from a center point & angle */
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

/* fill a vertex array with the midpoint */
void
midpoint(float array[], float a[], float b[])
{
  array[X] = ((a[X] + b[X]) / 2);
  array[Y] = ((a[Y] + b[Y]) / 2);
}

/* area of a triangle from its 3 points */
float
triangle_area(float x[], float y[], float z[])
{
  float a = distance(x, y);
  float b = distance(y, z);
  float c = distance(z, x);

  float s = (a + b + c) / 2;

  return sqrt(s * (s - a) * (s - b) * (s - c));
}
