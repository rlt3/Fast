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

  game->player   = construct_player();
  game->running  = true;
  game->speed    = BASE_SPEED;
  game->fuel     = 3;
  game->level    = 1;
  
  /*
   * TODO: Make sure time starts when player has full control
   */

  game->current_time = (long int) SDL_GetTicks();
  game->frame_time   = game->current_time;
  game->speed_time   = game->current_time;
  game->level_time   = game->current_time;

  game->level_duration = 5000;

  game->asteroid_interval     = 0;
  game->next_asteroid_time    = 0;
  game->num_asteroids         = 0;
  game->current_max_asteroids = 0;

  /* set input_frames so it rolls over to 0 on the first loop */
  game->past_input_frame  = (INPUT_FRAMES - 1);

  /* preset the input to 'nothing' */
  reset_saved_input(game);
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

  int asteroid_count = 1;

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
        asteroid_count = 1;

        for (j = 0; j < MAX_ASTEROIDS; j++) {
          if (asteroids[j] == NULL) { continue; }

          if (polygon_visible(*asteroids[j])) {
            asteroid_count++;
          }
        }

        /* Make a new asteroid if there aren't enough on screen */
        //if (asteroid_count < max_asteroids) {
          asteroids[i]->first_below = 1;
          add_asteroid(asteroids);
        //}

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
    void (*speed)(struct Game *), 
    void (*level)(struct Game *), 
    void (*update)(struct Game *), 
    void (*display)(struct Game *), 
    void (*restraint)(struct Game *, bool *))
{
  bool looping = true;

  while (game->running && looping) {
    game->current_time = (long int) SDL_GetTicks();

    /* make sure our input state is constantly updated */
    SDL_PollEvent(&game->event);

    /* update the speed of the game every second */
    //if (game->current_time - game->speed_time > ONE_SECOND) {
    //  game->speed_time = game->current_time;

    //  if (speed) {
    //    speed(game);
    //  }
    //}

    /* 
     * add new asteroids every level. 
     */
    //if (game->current_time - game->level_time > game->level_duration) {
    //  game->level_time = game->current_time;

      if (level) {
        level(game);
      }
    //}

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

/* Find the first empty spot and create an asteroid */
void
add_asteroid(struct Polygon *asteroids[])
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
end_restraint(struct Game *game, bool *loop)
{
  while (SDL_PollEvent(&game->event)){
    switch (game->event.type) {
      case SDL_KEYDOWN:
        /* reset the game to its original point */
        set_game(game);

        /* animate player to the starting position */
        main_loop(game, NULL, NULL, animation_update, NULL, 
            animation_restraint);

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
  /* Create the asteroids at a set pace based on time */
  if (game->num_asteroids < game->current_max_asteroids &&
      game->current_time - game->next_asteroid_time > game->asteroid_interval) {
    game->next_asteroid_time += game->asteroid_interval;

    printf("adding astroid | current max: %d\n", game->current_max_asteroids);
    add_asteroid(game->asteroids);
    game->num_asteroids++;
  }

  /* if this level's duration is up */
  if (game->current_time - game->level_time > game->level_duration) {
    main_loop(game, NULL, NULL, next_level_update, NULL, next_level_restraint);

    game->level++;
    game->speed = 0.05;

    game->level_duration += 10000;

    /* Set the time frames of when our intervals to create asteroids */
    game->level_time = game->current_time;
    game->current_max_asteroids = (game->level + 1) + game->level;
    game->asteroid_interval = (game->level_duration / 2) / game->current_max_asteroids;
    game->next_asteroid_time = game->level_time + game->asteroid_interval;

    /* Add an asteroid to get us started with this next level */
    add_asteroid(game->asteroids);
    game->num_asteroids = 0;

    printf("Next level: %d\n", game->level);
  }

  /*
   * Game should have levels starting at level 1 with 3 asteroids at lasts 15
   * seconds. Each additional level takes 5 more seconds to complete and adds
   * two more asteroids as well as additional speed.
   *
   * At the end of each level a `main_loop' function call happens where the 
   * player is animated back towards the starting position and then the 
   * asteroids start coming.
   */
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

void
main_speed(struct Game *game)
{
  game->speed += 0.01f;
}

void
main_restraint(struct Game *game, bool *looping)
{
  if (player_collision(game->asteroids, *game->player)) {
    if (game->fuel > 0) {
      /* if there was collision and there's fuel to rewind */
      game->fuel--;
      main_loop(game, replay_speed, NULL, replay_update, replay_display, 
          replay_restraint);
    } else {
      /* else show end screen and give choice to continue */
      main_loop(game, NULL, NULL, NULL, NULL, end_restraint);
    }
  } 
}

void
next_level_update(struct Game *game)
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (game->asteroids[i] == NULL) { continue; }

    /* if it is visible, move it down. else remove it */
    if (polygon_visible(*game->asteroids[i])) {
      game->asteroids[i]->y -= game->speed;
      update_vertices(game->asteroids[i]);
    } else {
      deconstruct_polygon(game->asteroids[i]);
      game->asteroids[i] = NULL;
    }
  }
}

void
next_level_restraint(struct Game *game, bool *looping)
{
  static Uint32 time  = 0;
  static bool   start = false;
  int i;

  /* only start count down once all asteroids are gone */
  if (time == 0) {
    for (i = 0; i < MAX_ASTEROIDS; i++) {
      if (game->asteroids[i] != NULL) { 
        if (polygon_visible(*game->asteroids[i])) {
          start = false;
          break;
        }
      }

      start = true;
    }
  }

  if (start) {
    start = false;
    time  = SDL_GetTicks();
  }

  if (time != 0 && SDL_GetTicks() - time > 5000) {
    start = true;
    time  = 0;
    *looping = false;
  }
}

/* 
 * TODO:
 *    Make speed rewind smoothly and then after replaying, speed up gracefully
 *    back to where it was, giving player time to adapt to rewinding.
 *
 *    Also, speed needs to match the speed of the asteroid as it was when it was
 *    moving downwards normally so the player's input in replay matches up to 
 *    the asteroid. 
 *
 *    Since we increment by 0.01, we need to decrement by 0.01 in reverse so
 *    that everything is in lockstep.
 */
void
replay_speed(struct Game *game)
{
  game->speed -= 0.01f;
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
replay_display(struct Game *game)
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
    game->level_time += 3000;

    game->next_asteroid_time = game->level_time + game->asteroid_interval;
    *looping   = false;
  }
}

void
cleanup_game(struct Game *game)
{
  SDL_Quit();
  destroy_vertices_array(screen_vertices(), 4);
  deconstruct_polygon(game->player);
  deconstruct_polygon_array(game->asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(game->stars,     MAX_STARS);
}
