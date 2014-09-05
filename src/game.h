#ifndef FAST_GAME_H
#define FAST_GAME_H

#include <SDL2/SDL.h>
#include <time.h>
#include <stdio.h>

#include "graphics.h"

#define MAX_ASTEROIDS    16
#define MAX_STARS        32
#define PLAYER_ANGLES    3
#define ASTEROID_ANGLES  4
#define STAR_ANGLES      4
#define BASE_SPEED       0.01f

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
void             handle_input(const Uint8 keystate[]);

void             pause(SDL_Event *event);
bool             player_collision(struct Polygon *asteroids[], 
                     struct Polygon player);
void             set_game(struct Polygon *player, 
                      struct Polygon *asteroids[], 
                      float *speed);

#endif
