#include <SDL2/SDL.h>
#include "polygon.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 512

void 
sdl_error(const char *type) 
{
  fprintf(stderr, "%s: %s\n", type, SDL_GetError());
  exit(1);
}

int
main() 
{
  SDL_Window   *screen;
  SDL_Renderer *render;
  SDL_Event     event;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    sdl_error("SDL_Init Error");
  }

  screen = SDL_CreateWindow("Fast", 
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  render = SDL_CreateRenderer(screen, -1, 
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (screen == NULL || render == NULL) {
    sdl_error("SDL_CreateWindowAndRenderer Error");
  }

  /* 
   * Create our polygon's vertices. Our coordinates don't matter right now, but 
   * our angles do. They determine the shape of our shape!
   */
  struct Vertex vertices[] = {
    {100, 100,   45, 100},
    {100, 100,    0,  75},
    {100, 100,  -45, 100},
    {100, 100, -135, 100},
    {100, 100,  180,  75},
    {100, 100,  135, 100}
  };

  struct Polygon player    = { 
    .center   = { SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0 },
    .vertices = vertices,
    .points   = 6
    //.radius   = 100
  };

  /* our game state (running?), speed of movement, and the last updated time */
  bool     game = true;
  int     speed = 5;
  Uint32   last = SDL_GetTicks();

  /* for incrementing in loops */
  int i, x, y;

  while (game) {

    while (SDL_PollEvent(&event)){
      switch (event.type) {
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_ESCAPE: case SDL_QUIT:
              game = false;
              break;
        }
      }
    }

    /* around 24 frames a second */
    if (SDL_GetTicks() - last > 40) {
      last = SDL_GetTicks();

      const Uint8 *keystate = SDL_GetKeyboardState(NULL);

      /* Movement up and down */
      if(keystate[SDL_SCANCODE_W])
        player.center = vertex_shift(player.center,  speed);

      if(keystate[SDL_SCANCODE_S])
        player.center = vertex_shift(player.center, -speed);

      /* Turning left or right */
      if(keystate[SDL_SCANCODE_A])
        player.center.angle = (player.center.angle + 2) % 360;

      if(keystate[SDL_SCANCODE_D])
        player.center.angle = (player.center.angle - 2) % 360;

      /* Speeding up ! */
      if(keystate[SDL_SCANCODE_LSHIFT])
        speed = 8;


      /* Update all the vertices of the polygon to new locations */
      for (i = 0; i < player.points; i++) {
        player.vertices[i] = vertex_point_to(player.center, player.vertices[i]);
      }

      /* Clear the screen */
      SDL_SetRenderDrawColor(render, 0, 0, 0, 255); 
      SDL_RenderClear(render);

      /* Get bounds which is set in order top 0, right 1, bottom 2, left 3 */
      polygon_bounds(player, player.bounds);

      /* Draw the bounds for fun */
      SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
      SDL_RenderDrawPoint(render, player.bounds[3], player.bounds[0]);
      SDL_RenderDrawPoint(render, player.bounds[1], player.bounds[2]);

      /* Fill our polygon ! */
      for (x = player.bounds[3]; x < player.bounds[1]; x++) {
        for (y = player.bounds[0]; y < player.bounds[2]; y++) {

          if (polygon_point_inside((struct Vertex){x, y}, 
                                   player.vertices, 
                                   player.points)) 
          {
            SDL_RenderDrawPoint(render, x, y);
          }

        }
      }

      SDL_RenderPresent(render);
    }

  }

  SDL_Quit();

  return EXIT_SUCCESS;
}
