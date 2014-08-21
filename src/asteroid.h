#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "polygon.h"

/* defines max random number for generating 0.0 to 1.0 */
#define ARC4RANDOM_MAX   0x100000000
#define MAX_ASTEROIDS    16
#define ASTEROID_ANGLES  4

struct Polygon * construct_asteroid();
void             handle_asteroids(struct Polygon *asteroids[], 
                                  struct Polygon player, 
                                  float speed);

void             deconstruct_asteroids(struct Polygon *p[]);
void             deconstruct_asteroid(struct Polygon *p);