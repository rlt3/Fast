#include "game.h"

int
initialize_game(struct Game *game)
{
  if (initialize_graphics(&game->graphics) != 0) {
    /* no graphics! */
    return 1;
  }

  srand(time(NULL));
  srand48(time(NULL));

  /* make all the stars now so they show up immediately */
  construct_all_stars(game->stars);

  game->player     = construct_player();
  game->running    = true;
  game->speed      = BASE_SPEED;
  game->frame_time = SDL_GetTicks();
  game->speed_time = game->frame_time;

  return 0;
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
    .center   = (struct Vertex){ 0.0f, -3.0f, 90 },
    .radius   = 0.75,
    .sides    = PLAYER_ANGLES
  };

  p->vertices = (struct Vertex*) malloc(sizeof(struct Vertex) * PLAYER_ANGLES);

  if (p->vertices == NULL) {
    fprintf(stderr, "%s\n", "Out of memory.");
    exit(1);
  }

  p->vertices[0] = (struct Vertex) { 0, 0,    0 };
  p->vertices[1] = (struct Vertex) { 0, 0, -135 };
  p->vertices[2] = (struct Vertex) { 0, 0,  135 };

  return p;
}

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

    /* if it ain't visible anymore, make a new one and set the y above screen */
    if (below_screen(*stars[i])) {
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

void
handle_input(const Uint8 keystate[])
{
}

/* pause until a button is hit */
void
pause(SDL_Event *event)
{
  while (1) {
    while (SDL_PollEvent(event)){
      switch (event->type) {
        case SDL_KEYDOWN:
          return;
      }
    }
  }
}

bool
player_collision(struct Polygon *asteroids[], 
                 struct Polygon  player)
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) { continue; }

    if (triangle_intersects_polygon(player, *asteroids[i])) {
      return true;
    }
  }
  return false;
}

/* set the game so it can be called to reset at any time */
void
set_game(struct Polygon *player, struct Polygon *asteroids[], float *speed)
{
  player->center.x =  0.0f;
  player->center.y = -3.0f;
  player->center.angle = 90;

  deconstruct_polygon_array(asteroids, MAX_ASTEROIDS);
  
  *speed = 0.01f;
}


int
display_game(struct Game *game)
{
  set_display();
  display_triangle(*game->player);
  display_quads(game->asteroids, MAX_ASTEROIDS);
  display_quads(game->stars,     MAX_STARS);
}

void
cleanup_game(struct Game *game)
{
  deconstruct_polygon_array(game->asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(game->stars,     MAX_STARS);
}
