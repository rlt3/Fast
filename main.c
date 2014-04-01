#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 512

void sdl_error(const char *type) {
  fprintf(stderr, "%s: %s\n", type, SDL_GetError());
  exit(1);
}

struct Vertex {
  float x;
  float y;
  int angle;
};

struct Vertex get_point(struct Vertex base, float radius, float angle) {
  return (struct Vertex) {
    .x = (base.x + (radius * sin(angle * (M_PI/180)))),
    .y = (base.y + (radius * cos(angle * (M_PI/180)))),
    .angle = angle
  };
}

int main() {
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

  bool          game   = true;

  struct Vertex center = { 
    .x = SCREEN_WIDTH/2,
    .y = SCREEN_HEIGHT/2,
    .angle = 0
  };

  float radius = 100;

  float mx, my, m, b;
  int   x, y;
  int   right, left;

  struct Vertex zero        = get_point(center, 30, 0);
  struct Vertex ninety      = get_point(center, 30, 90);
  struct Vertex one_eighty  = get_point(center, 30, 180);
  struct Vertex two_seventy = get_point(center, 30, 270);

  struct Vertex user        = get_point(center, radius, 90);
  
  while (game) {

    while (SDL_PollEvent(&event)){
      switch (event.type) {
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_ESCAPE: case SDL_QUIT:
              game = false;
              break;

            case SDLK_w: case SDLK_UP: case SDLK_k:
              center.angle = (center.angle + 1) % 360;
              break;

            case SDLK_a: case SDLK_LEFT: case SDLK_h:
              center.x--;
              break;

            case SDLK_s: case SDLK_DOWN: case SDLK_j:
              center.angle = (center.angle - 1) % 360;
              break;

            case SDLK_d: case SDLK_RIGHT: case SDLK_l:
              center.x++;
              break;
        }
      }
    }

    /* Update user */
    user = get_point(center, radius, center.angle);

    SDL_SetRenderDrawColor(render, 0, 0, 0, 255); 
    SDL_RenderClear(render);

    /* Draw Center point in white */
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderDrawPoint(render, center.x, center.y);

    /* Use slope intercept to get line's equation */
    my = (float)(center.y - user.y);
    mx = (float)(center.x - user.x);
    m  = (float)(my/mx);
    b  = center.y - (m * center.x);

    /* Find the bounds */
    if (center.x < user.x) {
      left  = center.x;
      right = user.x;
    } else {
      left  = user.x;
      right = center.x;
    }

    /* Draw Line to Point */
    for (x = left; x < right; x++) {
      y = (m*x) + b;

      SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
      SDL_RenderDrawPoint(render, x, y);
    }

    /* Draw end point in red */
    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
    SDL_RenderDrawPoint(render, user.x, user.y);

    SDL_RenderPresent(render);
  }
  
  SDL_Quit();

  return 0;
}
