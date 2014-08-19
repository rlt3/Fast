#include "asteroid.h"

/* create randomized asteroids to kill our player */
struct Polygon *
construct_asteroid() 
{
  /* allocate space for our asteroid on the heap */
  struct Polygon *p = (struct Polygon*) malloc(sizeof(struct Polygon));

  if (p == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }


  /* define the asteroid and allocate space for the angles array */
  *p = (struct Polygon) {
    .center = (struct Vertex){ 
      /* 
       * Generate a random number between 0.0f and 1.0f, multiply it
       * by 10 to get a range from 0.0f to 10.0f and then subtract 5.0f
       * to get a final range of -5.0f to 5.0f
       */
      .x = ((((double)arc4random() / ARC4RANDOM_MAX) * 10.0f) - 5.0f),
      .y = 4.0f,
    },
    .angle  = rand() % 91,
    .radius = 0.5,
    .angles = (float *) malloc(sizeof(float) * ASTEROID_ANGLES)
  };

  if (p->angles == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  /* Get a random number for each quadrant (0..90 and then add the angle) */
  p->angles[0] = rand() % 91;
  p->angles[1] = 90  + (rand() % 91);
  p->angles[2] = 180 + (rand() % 91);
  p->angles[3] = 270 + (rand() % 91);

  return p;
}


void
handle_asteroids(struct Polygon *asteroids[], 
                 struct Polygon player, 
                 float speed)
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) { continue; }

    asteroids[i]->center.y -= speed;

    /* if it ain't visible anymore, make a new one */
    if (!vertex_visible(asteroids[i]->center)) {
      deconstruct_asteroid(asteroids[i]);
      asteroids[i] = construct_asteroid();
      continue;
    }

    //if (polygon_point_inside(player.center, *asteroids[i], MAX_ASTEROIDS)) {
    //  printf("Collision!");
    //}
  }
}

/* Free the space of our asteroids */
void
deconstruct_asteroids(struct Polygon *p[])
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (p[i] == NULL) { continue; }
    deconstruct_asteroid(p[i]);
  }
}

void
deconstruct_asteroid(struct Polygon *p)
{
  free(p->angles);
  free(p);
}
