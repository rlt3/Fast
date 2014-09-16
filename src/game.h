#ifndef FAST_GAME_H
#define FAST_GAME_H

#include <time.h>
#include <stdbool.h>

#include "graphics.h"
#include "polygon.h"

#define MAX_ASTEROIDS    16
#define MAX_STARS        32

#define BASE_SPEED       0.01f
#define ONE_SECOND       1000
#define THIRTY_FPS       (ONE_SECOND/30)
#define FIFTEEN_FPS      (ONE_SECOND/15)

/* keep input for last 5 seconds */
#define INPUT_FRAMES     75

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

  Uint32          current_time;

  Uint32          frame_time;
  Uint32          speed_time;
  Uint32          input_time;

  int             input;

  int             past_input[INPUT_FRAMES];
  int             past_input_frame;

  SDL_Event       event;
};

int              initialize_game(struct Game *game);
void             construct_all_stars(struct Polygon *stars[]);

void             animate_player(struct Game *game);
void             time_animate_player(struct Game *game);

void             set_game(struct Polygon *player, 
                      struct Polygon *asteroids[], 
                      float *speed);

int              gather_input();
int              opposite_input(int input);
void             save_input(struct Game * game, int input);
void             handle_input(struct Game * game);

void             handle_asteroids(struct Polygon *asteroids[], float speed);
void             handle_stars(struct Polygon *stars[], float speed);

void             display_player(struct Game *game);
void             display_asteroids(struct Game *game);
void             display_stars(struct Game *game);
void             display_game(struct Game *game);

void             start_screen(struct Game *game);
void             pause_screen(SDL_Event *event);

bool             player_collision(struct Polygon *asteroids[], 
                     struct Polygon player);

void             cleanup_game(struct Game *game);



#endif
