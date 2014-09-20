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

  game->player   = construct_player();
  game->running  = true;
  game->speed    = BASE_SPEED;

  game->current_time = SDL_GetTicks();
  game->frame_time   = game->current_time;
  game->speed_time   = game->current_time;
  game->input_time   = game->current_time;
  game->level_time   = game->current_time;

  /* set input_frames so it rolls over to 0 on the first loop */
  game->past_input_frame  = (INPUT_FRAMES - 1);

  /* preset the input to 'nothing' */
  reset_saved_input(game);

  return 0;
}

void 
construct_all_stars(struct Polygon *stars[])
{
  int i;
  for (i = 0; i < MAX_STARS; i++) {
    stars[i] = construct_star();
  }
}

/* Get hard integer input codes so we can use them in other functins */
int
gather_input()
{
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  if (keystate[SDL_SCANCODE_W]) {
    return SDL_SCANCODE_W;
  }

  if (keystate[SDL_SCANCODE_A]) {
    return SDL_SCANCODE_A;
  }

  if (keystate[SDL_SCANCODE_S]) {
    return SDL_SCANCODE_S;
  }

  if (keystate[SDL_SCANCODE_D]) {
    return SDL_SCANCODE_D;
  }

  if (keystate[SDL_SCANCODE_ESCAPE]) {
    return SDL_SCANCODE_ESCAPE;
  }

  return NO_INPUT;
}

/* return the opposite: down becomes up, left becomes right */
int 
opposite_input(int input)
{
  switch(input) {
    case SDL_SCANCODE_W:
      return SDL_SCANCODE_S;
      break;

    case SDL_SCANCODE_S:
      return SDL_SCANCODE_W;
      break;

    case SDL_SCANCODE_A:
      return SDL_SCANCODE_D;
      break;

    case SDL_SCANCODE_D:
      return SDL_SCANCODE_A;
      break;

    default:
      return NO_INPUT;
  }
}

/* save the current input and to our past_input array */
void
save_input(struct Game *game, int input) 
{
  /* set the of the player */
  game->input = input;

  /* save the opposite of the player's input so we can 'replay' it */
  game->past_input_frame = ++game->past_input_frame % INPUT_FRAMES;
  game->past_input[game->past_input_frame] = opposite_input(input);
}

void
reset_saved_input(struct Game *game)
{
  int i;
  for (i = 0; i < INPUT_FRAMES; i++) {
    game->past_input[i] = NO_INPUT;
  }
}

void
handle_input(struct Game *game)
{
  game->player->angle = 90;

  switch(game->input) {
    case SDL_SCANCODE_W:
      game->player->y += 0.1f;
      break;

    case SDL_SCANCODE_S:
      game->player->y -= 0.1f;
      break;

    case SDL_SCANCODE_A:
      game->player->angle = 110;
      game->player->x -= game->speed + 0.25;
      break;

    case SDL_SCANCODE_D:
      game->player->angle = 70;
      game->player->x += game->speed + 0.25;
      break;

    case SDL_SCANCODE_ESCAPE:
      game->running = false;
      break;
  }

  update_vertices(game->player);
}

/*
 * TODO:
 *    Asteroids, after rewinding time, seem to appear at random. This occurs
 *    even if the player has already 'seen' that part of the game. So, it seems
 *    broken.
 *
 *    The problem lies in our update tick which probably shouldn't happen
 *    during our replay loop (so abstract it into a function) and also 'rewind'
 *    the time (our Uint32 variables) so it takes a couple of real seconds to
 *    create anything new.
 *
 *    Will probably need to set the "level_time" variable to `now' whenever the
 *    restraint for replaying ends.
 */

void
handle_asteroids(struct Polygon *asteroids[], float speed)
{
  int i;

  int backwards = (speed < 0 ? 1 : 0);

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) { continue; }

    asteroids[i]->y -= speed;

    /* if it is below the screen and we're not going backwards */
    if (below_screen(*asteroids[i]) && !backwards) {

      /* set the time it first went below the screen */
      if (asteroids[i]->first_below == 0) {
        asteroids[i]->first_below = SDL_GetTicks();
      }

      /* once it has been under 5 seconds, make a new one */
      if (SDL_GetTicks() - asteroids[i]->first_below > 5000) {

        deconstruct_polygon(asteroids[i]);
        asteroids[i] = construct_asteroid();
      }
    }

    update_vertices(asteroids[i]);
  }
}

void
handle_stars(struct Polygon *stars[], float speed)
{
  int i;

  /* if speed is less than 0, we're moving backwards */
  int backwards = speed < 0 ? 1 : 0;
  int offscreen = false;

  /* move slower than asteroids to simluate a parallax effect */
  speed = speed / 3.0f;
  
  for (i = 0; i < MAX_STARS; i++) {
    if (stars[i] == NULL) {
      stars[i] = construct_star();
    }

    stars[i]->y -= speed;

    /* if it went below the screen (or above the screen while backwards) */
    if (below_screen(*stars[i]) || (above_screen(*stars[i]) && backwards)) {
      deconstruct_polygon(stars[i]);
      stars[i] = construct_star();

      /* put it below the screen if backwards else above */
      stars[i]->y = backwards ? -5.0f : 5.0f;
    }

    update_vertices(stars[i]);
  }
}

void
display_player(struct Game *game)
{
  display_triangle(game->graphics.ship_texture, 
      game->player->x, 
      game->player->y, 
      game->player->vertices);
}

void
display_asteroids(struct Game *game)
{
  int i, j;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (game->asteroids[i] == NULL) { continue; }

    display_quad(game->graphics.asteroid_texture, 
        game->asteroids[i]->x, 
        game->asteroids[i]->y, 
        game->asteroids[i]->vertices);
  }
}

void
display_stars(struct Game *game)
{
  int i, j;
  for (i = 0; i < MAX_STARS; i++) {
    if (game->stars[i] == NULL) { continue; }

    display_quad(0, game->stars[i]->x, game->stars[i]->y, 
        game->stars[i]->vertices);
  }
}

void
display_essentials(struct Game *game)
{
  display_stars(game);
  display_asteroids(game);
  display_player(game);
}

bool
player_collision(struct Polygon *asteroids[], struct Polygon  player)
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

/*
 * The `main loop' of the game abstracted so we can use it for other things
 * while still keeping our timesteps synchronous. Mix & Match whichever
 * functions that will be called at the major steps to achieve cool things.
 *
 * Accepts NULL for every argument except the restraint.
 */
void
main_loop(struct Game *game, 
    void (*speed)(struct Game *), 
    void (*input)(struct Game *), 
    void (*level)(struct Game *), 
    void (*update)(struct Game *), 
    void (*display)(struct Game *), 
    void (*restraint)(struct Game *, bool *))
{
  bool looping = true;

  while (game->running && looping) {
    game->current_time = SDL_GetTicks();

    /* make sure our input state is constantly updated */
    SDL_PollEvent(&game->event);

    /* update the speed of the game every second */
    if (game->current_time - game->speed_time > ONE_SECOND) {
      game->speed_time = game->current_time;

      if (speed) {
        speed(game);
      }
    }

    /* update the level of the game every 1.5 seconds */
    if (game->current_time - game->level_time > 3500) {
      printf("%u - %u < 3500\n", game->current_time, game->level_time);
      game->level_time = game->current_time;

      if (level) {
        level(game);
      }
    }

    /* update the input of the game every 15 frames */
    if (game->current_time - game->input_time > FIFTEEN_FPS) { 
      game->input_time = game->current_time;

      if (input) {
        input(game);
      }
    }

    /* update the the game every 30 frames */
    if (game->current_time - game->frame_time > THIRTY_FPS) {
      game->frame_time = game->current_time;

      if (update) {
        update(game);
      }
    }

    /* set up the display for drawing */
    set_display();

    /* draw the player, stars, and asteroids */
    display_essentials(game);
    
    /* draw whatever else if something was provided */
    if (display) {
      display(game);
    }
    
    render(&game->graphics);

    /* stop whatever loop we're in (animation, replaying, or the game loop) */
    restraint(game, &looping);
  }
}

float**
logo_vertices()
{
  static bool created = false;
  static float **logo_vertices;

  /* return the same logo vertices every time */
  if (!created) {
    logo_vertices = make_vertices_array(4);

    logo_vertices[0][X] =  4.0;
    logo_vertices[0][Y] =  2.0;
    logo_vertices[1][X] = -4.0;
    logo_vertices[1][Y] =  2.0;
    logo_vertices[2][X] = -4.0;
    logo_vertices[2][Y] = -2.0;
    logo_vertices[3][X] =  4.0;
    logo_vertices[3][Y] = -2.0;

    created = true;
  }
  
  return logo_vertices;
}

void
start_update(struct Game *game)
{
  handle_stars(game->stars, game->speed);
}

void
start_display(struct Game *game)
{
  display_quad(game->graphics.main_screen_texture, 0, 0, logo_vertices());
}

void
start_restraint(struct Game *game, bool *loop)
{
  while (SDL_PollEvent(&game->event)){
    switch (game->event.type) {
      case SDL_KEYDOWN:
        *loop = false;
    }
  }
}

void
animation_update(struct Game *game)
{
  /* move towards the player starting position and don't go over it */
  if (game->player->y < -3.0f) {
    game->player->y += 0.05f;
  }

  handle_stars(game->stars, game->speed);
  update_vertices(game->player);
}

void
animation_restraint(struct Game *game, bool *loop)
{
  static Uint32 time;
  static bool   start = true;

  /* setup our initial time */
  if (start) {
    start = false;
    time  = SDL_GetTicks();
  }
  
  if (SDL_GetTicks() - time > 3000) {
    start = true;
    *loop = false;
  }
}

void
main_level(struct Game *game)
{
  /* find the next free spot and make an asteroids */
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (game->asteroids[i] == NULL) {
      game->asteroids[i] = construct_asteroid();
      game->asteroids[i]->first_below = 0;
      break;
    }
  }
}

void
main_update(struct Game *game)
{
  handle_input(game);
  handle_asteroids(game->asteroids, game->speed);
  handle_stars(game->stars, game->speed);
}

void
main_speed(struct Game *game)
{
  game->speed += 0.01f;
}

void
main_input(struct Game *game)
{
  save_input(game, gather_input());
}

void
main_restraint(struct Game *game, bool *looping)
{
  if (player_collision(game->asteroids, *game->player)) {
    main_loop(game, replay_speed, replay_input, NULL,
        replay_update, replay_display, replay_restraint);
  }
}

void
replay_speed(struct Game *game)
{
  game->speed -= 0.01f;
}

void
replay_input(struct Game *game)
{
  /* set the input from the player's past inputs and move backwards */
  game->input = game->past_input[game->past_input_frame];
  game->past_input_frame--;

  /* if the input frame has reach 0, loop it backwards */
  if (game->past_input_frame < 0) {
    game->past_input_frame = (INPUT_FRAMES - 1);
  }
}

void
replay_update(struct Game *game)
{
  /* like normal except we're moving backwards */
  handle_input(game);
  handle_asteroids(game->asteroids, -game->speed);
  handle_stars(game->stars, -game->speed);
}

void
replay_display(struct Game *game)
{
  display_quad(game->graphics.collision_texture, 0, 0, logo_vertices());
}

void
replay_restraint(struct Game *game, bool *looping)
{
  static int counter    = 0;
  static int last_frame = -1;

  /* setup initial counter */
  if (last_frame < 0) {
    counter    = 0;
    last_frame = game->past_input_frame;
  }

  /* if the input frame has changed */
  if (last_frame != game->past_input_frame) {
    last_frame = game->past_input_frame;
    counter++;
  }

  /* when all of the past input has been used, discard it and stop */
  if (counter > INPUT_FRAMES) {
    reset_saved_input(game);
    last_frame = -1;
    game->level_time = SDL_GetTicks();
    *looping   = false;
  }
}

void
cleanup_game(struct Game *game)
{
  SDL_Quit();
  destroy_vertices_array(logo_vertices(), 4);
  deconstruct_polygon(game->player);
  deconstruct_polygon_array(game->asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(game->stars,     MAX_STARS);
}
