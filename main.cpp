#include <iostream>

#include <SDL2/SDL.h>

void sdl_error(const char *type) {
  fprintf(stderr, "%s: %s\n", type, SDL_GetError());
  exit(1);
}

int main(int argc, char **argv) {

  SDL_Window   *screen;
  SDL_Renderer *render;
  SDL_Event     event;

  bool          game      = true;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    sdl_error("SDL_Init Error");
  }

  screen = SDL_CreateWindow("Fast", 
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      640, 512, SDL_WINDOW_SHOWN);

  render = SDL_CreateRenderer(screen, -1, 
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (screen == NULL || render == NULL) {
    sdl_error("SDL_CreateWindowAndRenderer Error");
  }

  /* Start time, add movement speed incrementally every N seconds */
  unsigned start_time = SDL_GetTicks();

  while (game) {

    /* Handle events */
    while (SDL_PollEvent(&event)){
      switch (event.type) {

        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_ESCAPE: case SDL_QUIT:
              game = false;
              break;
          }
          break;

      }
    }

    /* Set color to black and clear screen */
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render);

    /* Set color to white */
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

    /* Create an array of (x, y) coordinates and draw them */
    SDL_Point points[4] = { {30, 30}, {60, 30}, {30, 60}, {60, 60} };
    SDL_RenderDrawPoints(render, points, 4);

    SDL_RenderPresent(render);
  }

  SDL_Quit();
  
  return 0;
}
