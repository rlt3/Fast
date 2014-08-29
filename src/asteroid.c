#include "asteroid.h"

/* create randomized asteroids to kill our player */
struct Polygon *
construct_asteroid() 
{
  struct Polygon *p = (struct Polygon*) malloc(sizeof(struct Polygon));

  if (p == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  *p = (struct Polygon) {
    .center = (struct Vertex){ 
      /* 
       * Generate a random number between 0.0f and 1.0f, multiply it
       * by 10 to get a range from 0.0f to 10.0f and then subtract 5.0f
       * to get a final range of -5.0f to 5.0f -- the width of the screen
       */
      .x     = ((drand48() * 10.0f) - 5.0f),
      .y     = 5.0f,
      .angle = rand() % 91,
    },
    .radius = 0.5,
    .sides  = ASTEROID_ANGLES
  };

  p->vertices = (struct Vertex*) malloc(sizeof(struct Vertex) * ASTEROID_ANGLES);

  if (p->vertices == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  /* handle any polygon by splitting 360 degrees among the sides */
  int angle_portion = 360 / p->sides;

  /* make our asteroids random by generating an angle for each side */
  int i;
  for (i = 0; i < p->sides; i++) {
    p->vertices[i] = (struct Vertex){ 
      .x     = 0,
      .y     = 0,
      .angle = (angle_portion * i) + rand() % 91
    };
  }

  return p;
}

void
handle_asteroids(struct Polygon *asteroids[], 
                 float          speed)
{
  int i, j;

  /* the num asteroids are based on the speed, which increases as time goes */
  int num_asteroids = (int)(speed * 10) + 1;

  for (i = 0; i < num_asteroids; i++) {

    if (asteroids[i] == NULL) { 
      asteroids[i] = construct_asteroid();
    }

    asteroids[i]->center.y -= speed;

    /* if it ain't visible anymore off the bottom, make a new one */
    if (below_screen(*asteroids[i])) {
      deconstruct_polygon(asteroids[i]);
      asteroids[i] = construct_asteroid();
    }

    update_vertices(asteroids[i]);
  }
}
