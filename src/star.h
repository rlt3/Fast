#ifndef FAST_STAR_H
#define FAST_STAR_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "polygon.h"

#define MAX_STARS    32
#define STAR_ANGLES  4

struct Polygon * construct_star();
void             construct_all_stars(struct Polygon *stars[]);

void             deconstruct_stars(struct Polygon *p[]);
void             deconstruct_star(struct Polygon *p);

#endif
