#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "polygon.h"
#include "graphics.h"
#include "game.h"

int
main(int argc, char *argv[])
{
  struct Game     game;

  if (initialize_game(&game)) {
    exit(1);
  }

  while (game.running) {

    /* Every second increase the game's speed */
    if (SDL_GetTicks() - game.speed_time > 1000) {
      game.speed_time = SDL_GetTicks();
      game.speed      = game.speed + 0.01f;
    }

    while (SDL_PollEvent(&game.event)){
      switch (game.event.type) {
        case SDL_KEYDOWN:
          switch(game.event.key.keysym.sym) {
            case SDLK_ESCAPE: case SDL_QUIT:
              game.running = false;
              break;
        }
      }
    }

    /* around 24 frames a second */
    if (SDL_GetTicks() - game.frame_time > 40) {
      game.frame_time = SDL_GetTicks();

      const Uint8 *keystate = SDL_GetKeyboardState(NULL);

      /* reset to 90 degrees when player isn't turning */
      game.player->center.angle = 90;

      /* Movement up and down */
      if (keystate[SDL_SCANCODE_W]) {
        game.player->center = vertex_shift(game.player->center, 0.1f);
      }

      if (keystate[SDL_SCANCODE_S]) {
        game.player->center = vertex_shift(game.player->center, -0.1f);
      }

      /* Tilt the player left and move left */
      if (keystate[SDL_SCANCODE_A]) {
        game.player->center.angle = 110;
        game.player->center.x -= game.speed + 0.25;
      }

      /* Tilt the player right and move right */
      if (keystate[SDL_SCANCODE_D]) {
        game.player->center.angle = 70;
        game.player->center.x += game.speed + 0.25;
      }

      handle_asteroids(game.asteroids, game.speed);
      handle_stars(game.stars, game.speed);
      update_vertices(game.player);

      set_display();

      display_triangle(*game.player);
      display_quads(game.asteroids, MAX_ASTEROIDS);
      display_quads(game.stars,     MAX_STARS);

      render(&game.graphics);

      /* do collision after rendering so the player can visually see it */
      if (player_collision(game.asteroids, *game.player)) {
        pause(&game.event);
        set_game(game.player, game.asteroids, &game.speed);
      }
    }
  }
  
  SDL_Quit();
  deconstruct_polygon_array(game.asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(game.stars,     MAX_STARS);

  return 0;
}
