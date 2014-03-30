#include <stdio.h>
#include <SDL2/SDL.h>

#include "Direction.hpp"
#include "Movement.hpp"
#include "Polygon.hpp"

void sdl_error(const char *type) {
  fprintf(stderr, "%s: %s\n", type, SDL_GetError());
  exit(1);
}

int main() {
  SDL_Window   *screen;
  SDL_Renderer *render;
  SDL_Event     event;

  Movement      vector(10, 10, 5);
  Polygon       polygon;

  bool          game = true;

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

  while (game) {
    vector.update(SDL_GetTicks());
    vector.accelerating(true);

    /* Set color to black and clear screen */
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render);

    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

    SDL_RenderDrawPoints(render, 
        (SDL_Point*)polygon.vertices(), polygon.points());

    SDL_RenderPresent(render);

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

    Location loc = vector.location();
  }

  Point* vertices = polygon.vertices();

  for (int i = 0; i < polygon.points(); i++) {
    printf("(%d, %d)\n", vertices[i].x, vertices[i].y);
  }

  //vector.turn([&](direction) {
  //  case SDLK_w: case SDLK_UP: case SDLK_k:
  //    player.set_state(Direction::UP);
  //    break;

  //  case SDLK_a: case SDLK_LEFT: case SDLK_h:
  //    player.set_state(Direction::LEFT);
  //    break;

  //  case SDLK_s: case SDLK_DOWN: case SDLK_j:
  //    player.set_state(Direction::DOWN);
  //    break;

  //  case SDLK_d: case SDLK_RIGHT: case SDLK_l:
  //    player.set_state(Direction::RIGHT);
  //    break;
  //});
  
  SDL_Quit();

  return 0;
}
