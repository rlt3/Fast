#include "game.h"

int
main(int argc, char *argv[])
{
  struct Game game;

  if (initialize_game(&game) != 0) {
    exit(1);
  }

  while (game.running) {

    game.current_time = SDL_GetTicks();

    if (game.current_time - game.speed_time > ONE_SECOND) {
      game.speed_time = game.current_time;
      game.speed      = game.speed + 0.01f;
    }

    if (game.current_time - game.input_time > 66) { // double frame time
      game.input_time = game.current_time;
      game.input = gather_input();
    }

    /* make sure our input state is constantly updated */
    SDL_PollEvent(&game.event);

    if (game.current_time - game.frame_time > THIRTY_FPS) {
      game.frame_time = game.current_time;

      handle_input(&game);

      handle_asteroids(game.asteroids, game.speed);
      handle_stars(game.stars, game.speed);
    }

    display_game(&game);

    /* do collision after rendering so the player can visually see it */
    if (player_collision(game.asteroids, *game.player)) {
      pause_screen(&game.event);
      set_game(game.player, game.asteroids, &game.speed);
    }
  }

  cleanup_game(&game);

  return 0;
}
