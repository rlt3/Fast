#include "game.h"

int
main(int argc, char *argv[])
{
  struct Game game;

  if (initialize_game(&game) != 0) {
    exit(1);
  }
  
  main_loop(&game, main_speed, main_input, main_update, main_restraint);

  cleanup_game(&game);

  return 0;
}
