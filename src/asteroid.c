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
      .y     = 4.0f,
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
                 struct Polygon player, 
                 int            level)
{
  int   i, j;
  bool  visible;
  float speed = (float)level / 10;

  for (i = 0; i < level; i++) {
    /* create asteroids -- one for each level */
    if (asteroids[i] == NULL) { 
      asteroids[i] = construct_asteroid();
    }

    asteroids[i]->center.y -= speed;

    /* check for visibility at all points */
    visible = false;
    for (j = 0; j < asteroids[i]->sides; j++) {
      if (vertex_visible(asteroids[i]->vertices[j])) {
        visible = true;
        break;
      }
    }
    
    /* if it ain't visible anymore, make a new one */
    if (!visible) {
      deconstruct_asteroid(asteroids[i]);
      asteroids[i] = construct_asteroid();
    }

    update_vertices(asteroids[i]);
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
  free(p->vertices);
  free(p);
}
