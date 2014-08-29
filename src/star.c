#include "star.h"

struct Polygon *
construct_star()
{
  struct Polygon *p = (struct Polygon*) malloc(sizeof(struct Polygon));

  if (p == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  *p = (struct Polygon) {
    .center = (struct Vertex){ 
      /* Random point on the screen, facing up */
      .x     = ((drand48() * 10.0f) - 5.0f),
      .y     = ((drand48() *  9.0f) - 4.5f),
      .angle = 0,
    },
    .radius = 0.025,
    .sides  = STAR_ANGLES
  };

  p->vertices = (struct Vertex*) malloc(sizeof(struct Vertex) * STAR_ANGLES);

  if (p->vertices == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  /* handle any polygon by splitting 360 degrees among the sides */
  int angle_portion = 360 / p->sides;

  /* all of our stars are just rectangles/squares */
  p->vertices[0] = (struct Vertex) { 0, 0,  45 };
  p->vertices[1] = (struct Vertex) { 0, 0, 135 };
  p->vertices[2] = (struct Vertex) { 0, 0,-135 };
  p->vertices[3] = (struct Vertex) { 0, 0, -45 };

  return p;
}

void
handle_stars(struct Polygon *stars[], float speed)
{
  int i;

  /* move slower than asteroids to simluate a parallax effect */
  speed = speed / 3.0f;
  
  for (i = 0; i < MAX_STARS; i++) {
    if (stars[i] == NULL) {
      stars[i] = construct_star();
    }

    /* if it ain't visible anymore, loop it back to the top */
    if (below_screen(*stars[i])) {
      //stars[i]->center.y = -stars[i]->center.y;
      deconstruct_polygon(stars[i]);
      stars[i] = construct_star();
      stars[i]->center.y = 5.0f;
    }

    stars[i]->center.y -= speed;

    update_vertices(stars[i]);
  }
}

void 
construct_all_stars(struct Polygon *stars[])
{
  int i;
  for (i = 0; i < MAX_STARS; i++) {
    stars[i] = construct_star();
    update_vertices(stars[i]);
  }
}
