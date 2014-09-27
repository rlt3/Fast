#ifndef FAST_GAME_H
#define FAST_GAME_H

#include <time.h>
#include <stdbool.h>

#include "graphics.h"
#include "polygon.h"

#define MAX_ASTEROIDS    32
#define MAX_STARS        32

#define BASE_SPEED       0.01f
#define ONE_SECOND       1000
#define THIRTY_FPS       (ONE_SECOND/30)
#define FIFTEEN_FPS      (ONE_SECOND/15)

/* keep input for last 3 seconds */
#define INPUT_FRAMES     90
#define NO_INPUT         -1

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
  int             fuel;

  Uint32          current_time;

  Uint32          frame_time;
  Uint32          speed_time;
  Uint32          input_time;
  Uint32          level_time;

  int             input;

  int             past_input[INPUT_FRAMES];
  int             past_input_frame;

  SDL_Event       event;
};

int  initialize_game(struct Game *game);
void set_game(struct Game *game);
void construct_all_stars(struct Polygon *stars[]);

/* gather input, opposite it for replay and reset when done */
int  gather_input();
int  opposite_input(int input);
void reset_saved_input(struct Game *game);

/* handle the objects of our game */
void handle_input(struct Game * game);
void handle_asteroids(struct Polygon *asteroids[], float speed);
void handle_stars(struct Polygon *stars[], float speed);

void display_player(struct Game *game);
void display_asteroids(struct Game *game);
void display_stars(struct Game *game);
void display_essentials(struct Game *game);

bool player_collision(struct Polygon *asteroids[], 
         struct Polygon player);

/* create and always return the same static screen vertices */
float** screen_vertices();

/* find first empty spot and create an asteroid */
void add_asteroid(struct Polygon *asteroid[]);

/* main loop used for animation, replaying, and the main loop */
void main_loop(struct Game *game, 
        void (*speed)(struct Game *), 
        void (*level)(struct Game *), 
        void (*update)(struct Game *), 
        void (*display)(struct Game *), 
        void (*restraint)(struct Game *, bool *));

/* the functions which are passed into the main loop */
void start_update(struct Game *game);
void start_display(struct Game *game);
void start_restraint(struct Game *game, bool *loop);

void end_restraint(struct Game *game, bool *loop);

void animation_update(struct Game *game);
void animation_restraint(struct Game *game, bool *loop);

void main_speed(struct Game *game);
void main_level(struct Game *game);
void main_update(struct Game *game);
void main_restraint(struct Game *game, bool *looping);

void replay_speed(struct Game *game);
void replay_update(struct Game *game);
void replay_display(struct Game *game);
void replay_restraint(struct Game *game, bool *looping);

void cleanup_game(struct Game *game);

#endif
