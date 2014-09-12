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

  game->current_time = SDL_GetTicks();
  game->frame_time   = game->current_time;
  game->speed_time   = game->current_time;
  game->input_time   = game->current_time;

  start_screen(game);
  animate_player(game);

  return 0;
}

void
handle_asteroids(struct Polygon *asteroids[], float speed)
{
  int i, j;

  /* the num asteroids are based on the speed, which increases as time goes */
  int num_asteroids = (int)(speed * 10) + 1;

  for (i = 0; i < num_asteroids; i++) {

    if (asteroids[i] == NULL) { 
      asteroids[i] = construct_asteroid();
    }

    //asteroids[i]->center.y -= speed;
    asteroids[i]->y -= speed;

    /* if it ain't visible anymore off the bottom, make a new one */
    if (below_screen(*asteroids[i])) {
      deconstruct_polygon(asteroids[i]);
      asteroids[i] = construct_asteroid();
    }

    update_vertices(asteroids[i]);
  }
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

    stars[i]->y -= speed;

    /* if it ain't visible anymore, make a new one and set the y above screen */
    if (below_screen(*stars[i])) {
      deconstruct_polygon(stars[i]);
      stars[i] = construct_star();
      stars[i]->y = 5.0f;
    }

    update_vertices(stars[i]);
  }
}

void 
construct_all_stars(struct Polygon *stars[])
{
  int i;
  for (i = 0; i < MAX_STARS; i++) {
    stars[i] = construct_star();
  }
}

/* main menu start screen */
void
start_screen(struct Game *game)
{
  bool pre_game = true;

  float **logo_vertices = make_vertices_array(4);
  
  logo_vertices[0][X] =  4.0;
  logo_vertices[0][Y] =  2.0;
  logo_vertices[1][X] = -4.0;
  logo_vertices[1][Y] =  2.0;
  logo_vertices[2][X] = -4.0;
  logo_vertices[2][Y] = -2.0;
  logo_vertices[3][X] =  4.0;
  logo_vertices[3][Y] = -2.0;

  while (pre_game) {
    game->current_time = SDL_GetTicks();

    while (SDL_PollEvent(&game->event)){
      switch (game->event.type) {
        case SDL_KEYDOWN:
          pre_game = false;
      }
    }

    /* around 30 frames per second */
    if (game->current_time - game->frame_time > THIRTY_FPS) {
      game->frame_time = game->current_time;

      handle_stars(game->stars, game->speed);
    }

    set_display();
    display_stars(game);

    display_quad(game->graphics.main_screen_texture, 0, 0, logo_vertices);

    render(&game->graphics);
  }

  destroy_vertices_array(logo_vertices, 4);
}

/* pause until a button is hit */
void
pause_screen(SDL_Event *event)
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

/* set the game so it can be called to reset at any time */
void
set_game(struct Polygon *player, struct Polygon *asteroids[], float *speed)
{
  player->x =  0.0f;
  player->y = -3.0f;
  player->angle = 90;

  deconstruct_polygon_array(asteroids, MAX_ASTEROIDS);
  
  *speed = 0.01f;
}

void
display_game(struct Game *game)
{
  set_display();

  display_stars(game);
  display_player(game);
  display_asteroids(game);

  render(&game->graphics);
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
cleanup_game(struct Game *game)
{
  SDL_Quit();
  deconstruct_polygon_array(game->asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(game->stars,     MAX_STARS);
}

/*
 * if we are going to 'save' past input from a player and then play it back
 * to them at any given point, we need to make sure we take and use that
 * input in a manageable timestep so that when we go backwards we can 
 * accurately reproduce the movement in time.
 */
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

  return -1;
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

void
animate_player(struct Game *game)
{
  int time = 5; /* animate over 5 seconds */

  /*
   * instead of a destination point, give input like the player would so we can
   * start handling 'rewinding'. So, since we want to move up at this time we
   * would just give 5 up inputs to this animation function.
   *
   * The same function that handles input by the player can handle input by us
   * as well.
   *
   * For this to work, also, the player needs to be 'spawned' under the screen
   * too.
   *
   * So, all in all, this function should accept some sort of stack object (last
   * in first out) of input and the game pointer and should pop the stack and 
   * use that as if the player was inputting it themselves.
   */

  bool time_left = true;

  Uint32 start_time = game->current_time;

  /* use the same loop as normal, except player cannot interact */
  while (time_left) {
    game->current_time = SDL_GetTicks();

    /* keep our times current */
    if (game->current_time - game->speed_time > ONE_SECOND) {
      game->speed_time = game->current_time;
    }

    if (game->current_time - game->input_time > 66) { // double frame time
      game->input_time = game->current_time;
    }

    /* after 5 seconds, end loop */
    if (game->current_time - start_time > 5000) {
      time_left = false;
    }

    /* around 30 frames per second */
    if (game->current_time - game->frame_time > 33) {
      game->frame_time = game->current_time;

      /* move towards the player starting position and don't go over it */
      if (game->player->y < -3.0f) {
        game->player->y += 0.05f;
      }

      /*
       * update player's ship making sure it updates every second in accordance
       * with the time limit. Meaning it better be at the destination in the 5
       * seconds given.
       */

      handle_stars(game->stars, game->speed);
      update_vertices(game->player);
    }

    set_display();
    display_player(game);
    display_stars(game);
    render(&game->graphics);
  }
}
