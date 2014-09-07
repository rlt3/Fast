#ifndef FAST_GAME_H
#define FAST_GAME_H

#include <time.h>
#include <stdio.h>

#include "graphics.h"

#define MAX_ASTEROIDS    16
#define MAX_STARS        32

#define PLAYER_ANGLES    3
#define ASTEROID_ANGLES  4
#define STAR_ANGLES      4

#define BASE_SPEED       0.01f
#define ONE_SECOND       1000
#define THIRTY_FPS       (ONE_SECOND/30)

/*
 * Handle the state of the game including events such as input or collision.
 */

struct Game {
  struct Polygon *stars[MAX_STARS];
  struct Polygon *asteroids[MAX_STARS];
  struct Polygon *player;

  struct Graphics graphics;

  bool            running;
  float           speed;

  Uint32          frame_time;
  Uint32          speed_time;

  SDL_Event       event;
};

int              initialize_game(struct Game *game);
struct Polygon * construct_player();
struct Polygon * construct_asteroid();
struct Polygon * construct_star();
void             handle_asteroids(struct Polygon *asteroids[], float speed);
void             handle_stars(struct Polygon *stars[], float speed);
void             construct_all_stars(struct Polygon *stars[]);

void             pause(SDL_Event *event);
bool             player_collision(struct Polygon *asteroids[], 
                     struct Polygon player);
void             set_game(struct Polygon *player, 
                      struct Polygon *asteroids[], 
                      float *speed);
void             cleanup_game(struct Game *game);

int              gather_input();
void             handle_input(struct Game * game, int input);
void             animate_player(struct Game *game);

void             display_game(struct Game *game);

#endif
