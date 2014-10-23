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

  set_game(game);

  return 0;
}

void
set_game(struct Game *game)
{
  /* remove player if it exists */
  if (game->player != NULL) {
    deconstruct_polygon(game->player);
  }

  deconstruct_polygon_array(game->asteroids, MAX_ASTEROIDS);

  game->player  = construct_player();
  game->running = true;
  game->speed   = 0.05;
  game->fuel    = 3;
  game->level   = 1;

  set_time(game);
  set_level(game);

  /* set input_frames so it rolls over to 0 on the first loop */
  game->past_input_frame  = (INPUT_FRAMES - 1);

  /* preset the input to 'nothing' */
  reset_saved_input(game);
}

void
set_time(struct Game *game)
{
  game->current_time = (long int) SDL_GetTicks();
  game->frame_time   = game->current_time;
  game->speed_time   = game->current_time;
  game->level_time   = game->current_time;
}

/* Based on the duration (10 seconds) split the duration in half (5 seconds).
 * We want to have the maximum number of asteroids out by 5 seconds, but deploy
 * them in a manageable and realistic way (not all at once). 
 *
 * This is so they can be dodged one-by-one and also so that for half the level 
 * all of the asteroids are there.
 *
 * So, we split that half duration (5 seconds) into parts equal to the number
 * of asteroids. That is the asteroid interval and we deploy a new asteroid
 * every interval.
 */
void
set_level(struct Game *game)
{
  game->speed                += 0.05;
  game->level_duration       += 10000;
  game->level_time            = game->current_time;
  game->current_max_asteroids = (game->level + 1);
  game->asteroid_interval     = 
      (game->level_duration / 2) / game->current_max_asteroids;

  game->next_asteroid_time    = game->level_time + game->asteroid_interval;

  /* go ahead and add an asteroid for immediate play */
  make_asteroid(game->asteroids);
  game->num_asteroids         = 1;
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

  /* Move player and make sure they can't go too far off screen */

  switch(game->input) {
    case SDL_SCANCODE_W:
      if (game->player->y + 0.1f < Y_LENGTH) {
        game->player->y += 0.1f;
      }
      break;

    case SDL_SCANCODE_S:
      if (game->player->y - 0.1f > -Y_LENGTH) {
        game->player->y -= 0.1f;
      }
      break;

    case SDL_SCANCODE_A:
      if (game->player->x - (game->speed + 0.25f) > -(X_LENGTH - 1.5f)) {
        game->player->x -= game->speed + 0.25;
      }
      game->player->angle = 110;
      break;

    case SDL_SCANCODE_D:
      if (game->player->x + (game->speed + 0.25f) < (X_LENGTH - 1.5f)) {
        game->player->x += game->speed + 0.25;
      }
      game->player->angle = 70;
      break;
  }

  update_vertices(game->player);
}

/*
 * We always want an asteroid to be on screen to challenge player. We make a new
 * asteroid once an asteroid leaves. Since asteroids can move off screen and 
 * back on screen, we have to check if they have gone past once.
 *
 * Once off screen we hold onto them for 5 seconds so rewinding time show them.
 */

void
handle_asteroids(struct Polygon *asteroids[], float speed, int max_asteroids)
{
  int i, j;

  /* if speed is less than 0, we're moving backwards */
  int backwards = speed < 0 ? 1 : 0;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) { continue; }

    asteroids[i]->y -= speed;
    update_vertices(asteroids[i]);

    /* if it is on screen and we're going backwards */
    if (polygon_visible(*asteroids[i]) && backwards) {

      /* if the asteroid has come back above screen, reset its time */
      if (asteroids[i]->below_time != 0) {
        asteroids[i]->below_time = 0;
      }
    }

    /* if it is below the screen and we're not going backwards */
    if (below_screen(*asteroids[i]) && !backwards) {

      /* if the asteroid is coming in fresh, set its timer */
      if (asteroids[i]->below_time == 0) {
        asteroids[i]->below_time = SDL_GetTicks();
      }

      /* if it's the first time below screen, make a new asteroid */
      if (asteroids[i]->first_below == 0) {
        asteroids[i]->first_below = 1;

        /* Make a new asteroid if there aren't enough on screen */
        if (num_visible_asteroids(asteroids) < max_asteroids) {
          make_asteroid(asteroids);
        }
      }

      /* once it has been under 5 seconds, remove it leaving open spot */
      if (SDL_GetTicks() - asteroids[i]->below_time > 5000) {
        deconstruct_polygon(asteroids[i]);
        asteroids[i] = NULL;
      }
    }
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
    void (*level)(struct Game *), 
    void (*update)(struct Game *), 
    void (*display)(struct Game *), 
    void (*restraint)(struct Game *, bool *))
{
  bool looping = true;

  while (game->running && looping) {
    game->current_time = (long int) SDL_GetTicks();

    /* make sure our input state is constantly updated & able to quit anytime */
    while (SDL_PollEvent(&game->event)){
      switch (game->event.type) {
        case SDL_KEYDOWN:
          switch(game->event.key.keysym.sym) {
            case SDLK_ESCAPE: case SDL_QUIT:  
              game->running = false;
          }
      }
    }

    /* handle level information if needed */
    if (level) {
      level(game);
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

void
display_number(struct Game *game, int number)
{
  char string[8];

  sprintf(string, "%d", number);

  int i;
  int digit;
  int offset;
  float kerning;

  int length = strlen(string);

  for (i = 0; i < length; i++) {
    digit   = (int) string[i] - 48;
    kerning = 1.5 + (float) i;

    glPushMatrix();
    glTranslatef(kerning, 0.6f, 0.0f);
    display_quad(game->graphics.number_textures[digit], 0, 0, number_vertices());
    glPopMatrix();
  }
}

/* 
 * return the vertices for a rectangle the size of the screen. this is for
 * displaying overlays like the main menu screen/pause screen/interface for
 * the game using one set vertices.
 */
float**
screen_vertices()
{
  static bool created = false;
  static float **screen_vertices;

  /* return the same logo vertices every time */
  if (!created) {
    screen_vertices = make_vertices_array(4);

    screen_vertices[0][X] =  6.0;
    screen_vertices[0][Y] =  4.0;
    screen_vertices[1][X] = -6.0;
    screen_vertices[1][Y] =  4.0;
    screen_vertices[2][X] = -6.0;
    screen_vertices[2][Y] = -4.0;
    screen_vertices[3][X] =  6.0;
    screen_vertices[3][Y] = -4.0;

    created = true;
  }
  
  return screen_vertices;
}

float**
number_vertices()
{
  static bool created = false;
  static float **number_vertices;

  /* return the same logo vertices every time */
  if (!created) {
    number_vertices = make_vertices_array(4);

    number_vertices[0][X] =  0.65;
    number_vertices[0][Y] =  0.65;
    number_vertices[1][X] = -0.65;
    number_vertices[1][Y] =  0.65;
    number_vertices[2][X] = -0.65;
    number_vertices[2][Y] = -0.65;
    number_vertices[3][X] =  0.65;
    number_vertices[3][Y] = -0.65;

    created = true;
  }
  
  return number_vertices;
}

/* count how many asteroids are visible */
int
num_visible_asteroids(struct Polygon *asteroids[])
{
  int i;
  int asteroid_count = 0;

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) { continue; }

    if (polygon_visible(*asteroids[i])) {
      asteroid_count++;
    }
  }

  return asteroid_count;
}

/* Find the first empty spot and create an asteroid */
void
make_asteroid(struct Polygon *asteroids[])
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) {
      asteroids[i] = construct_asteroid();
      break;
    }
  }
}

void
end_display(struct Game *game)
{
  display_quad(game->graphics.score_texture, 0, 0, screen_vertices());
  display_number(game, game->level * 20);
}

void
end_restraint(struct Game *game, bool *loop)
{
  while (SDL_PollEvent(&game->event)){
    switch (game->event.type) {
      case SDL_KEYDOWN:
        /* reset the game to its original point */
        set_game(game);

        /* animate player to the starting position */
        main_loop(game, NULL, animation_update, NULL, animation_restraint);

        *loop = false;
    }
  }
}

void
start_update(struct Game *game)
{
  handle_stars(game->stars, game->speed);
}

void
start_display(struct Game *game)
{
  display_quad(game->graphics.main_screen_texture, 0, 0, screen_vertices());
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
  int i;
  int asteroid_count = 0;

  /* Create the asteroids at a set pace based on time */
  if (game->num_asteroids < game->current_max_asteroids &&
      game->current_time - game->next_asteroid_time > game->asteroid_interval) {
    game->next_asteroid_time += game->asteroid_interval;

    make_asteroid(game->asteroids);
    game->num_asteroids++;
  }

  /* if this level's duration is up */
  if (game->current_time - game->level_time > game->level_duration) {

    /* no more new asteroids now that the level is up */
    game->current_max_asteroids = 0;

    for (i = 0; i < MAX_ASTEROIDS; i++) {
      if (game->asteroids[i] == NULL) { continue; }

      if (!below_screen(*game->asteroids[i])) {
        asteroid_count++;
      }
    }

    /* once all asteroids have gone below screen from previous level */
    if (asteroid_count == 0) {
      game->level++;

      main_loop(game, NULL, main_update, next_level_display, 
          three_second_restraint);

      /* Set the time frames of when our intervals to create asteroids */
      set_level(game);
    }
  }
}

void
main_update(struct Game *game)
{
  /* set the of the player */
  game->input = gather_input();

  handle_input(game);

  /* save the opposite of the player's input so we can 'replay' it */
  game->past_input_frame = ++game->past_input_frame % INPUT_FRAMES;
  game->past_input[game->past_input_frame] = opposite_input(game->input);

  handle_asteroids(game->asteroids, game->speed, game->current_max_asteroids);
  handle_stars(game->stars, game->speed);
}

/* Handle collision and fuel deduction. */
void
main_restraint(struct Game *game, bool *looping)
{
  long int start;

  if (player_collision(game->asteroids, *game->player)) {
    if (game->fuel > 0) {
      /* if there was collision and there's fuel to rewind */
      game->fuel--;

      /* show what happened for 3 seconds */
      main_loop(game, NULL, NULL, collision_display, 
          three_second_restraint);

      /* then replay */
      main_loop(game, NULL, replay_update, NULL, replay_restraint);

    } else {
      /* else show end screen and give choice to continue */
      main_loop(game, NULL, NULL, end_display, end_restraint);
    }
  } 
}

void
next_level_display(struct Game *game)
{
  display_quad(game->graphics.level_texture, 0, 0, screen_vertices());
  display_number(game, game->level);
}

/* count for 3 seconds and then return to main loop */
void
three_second_restraint(struct Game *game, bool *looping)
{
  static Uint32 time  = 0;
  static bool   start = true;

  if (start) {
    start = false;
    time  = SDL_GetTicks();
  }

  if (time != 0 && SDL_GetTicks() - time > 3000) {
    start = true;
    time  = 0;
    *looping = false;
  }
}

void
replay_update(struct Game *game)
{
  /* set the input from the player's past inputs and move backwards */
  game->input = game->past_input[game->past_input_frame];
  game->past_input_frame--;

  /* if the input frame has reached 0, loop it backwards */
  if (game->past_input_frame < 0) {
    game->past_input_frame = (INPUT_FRAMES - 1);
  }

  /* like normal except we're moving backwards */
  handle_input(game);
  handle_asteroids(game->asteroids, -game->speed, game->current_max_asteroids);
  handle_stars(game->stars, -game->speed);
}

void
collision_display(struct Game *game)
{
  display_quad(game->graphics.collision_texture, 0, 0, screen_vertices());
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
  if (counter >= INPUT_FRAMES) {
    reset_saved_input(game);
    last_frame = -1;
    
    /* increase the time by the time we took here */
    game->level_time += 3200;
    game->next_asteroid_time += 3200;

    *looping   = false;
  }
}

void
cleanup_game(struct Game *game)
{
  SDL_Quit();
  destroy_vertices_array(screen_vertices(), 4);
  destroy_vertices_array(number_vertices(), 4);
  deconstruct_polygon(game->player);
  deconstruct_polygon_array(game->asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(game->stars,     MAX_STARS);
}
