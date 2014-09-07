#include "game.h"

int
main(int argc, char *argv[])
{
  struct Game game;

  if (initialize_game(&game)) {
    exit(1);
  }

  int input;

  Uint32 current_time = SDL_GetTicks();
  Uint32 input_frames = current_time;

  while (game.running) {

    current_time = SDL_GetTicks();

    if (current_time - game.speed_time > ONE_SECOND) {
      game.speed_time = SDL_GetTicks();
      game.speed      = game.speed + 0.01f;
    }

    /*
     * if we are going to 'save' past input from a player and then play it back
     * to them at any given point, we need to make sure we take and use that
     * input in a manageable timestep so that when we go backwards we can 
     * accurately reproduce the movement in time.
     */
    if (current_time - input_frames > 66) { // double frame time
      input_frames = SDL_GetTicks();
      input = gather_input();
    }

    /* make sure our input state is constantly updated */
    SDL_PollEvent(&game.event);

    if (current_time - game.frame_time > THIRTY_FPS) {
      game.frame_time = SDL_GetTicks();

      handle_input(&game, input);

      handle_asteroids(game.asteroids, game.speed);
      handle_stars(game.stars, game.speed);
      update_vertices(game.player);
    }

    display_game(&game);

    /* do collision after rendering so the player can visually see it */
    if (player_collision(game.asteroids, *game.player)) {
      pause(&game.event);
      set_game(game.player, game.asteroids, &game.speed);
    }
  }

  cleanup_game(&game);

  return 0;
}
