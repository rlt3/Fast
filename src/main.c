#include "game.h"

int
main(int argc, char *argv[])
{
  struct Game game;

  if (initialize_game(&game) != 0) {
    exit(1);
  }

  /* the start screen */
  main_loop(&game, NULL, start_update, start_display, start_restraint);
  
  /* animate player to the starting position */
  main_loop(&game, NULL, animation_update, NULL, animation_restraint);

  /* set the timeframes so that they start when player has full control */
  set_time(&game);
  
  /* main game loop */
  main_loop(&game, main_level, main_update, NULL, main_restraint);

  cleanup_game(&game);

  return 0;
}
