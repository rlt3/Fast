#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 512

void 
sdl_error(const char *type) {
  fprintf(stderr, "%s: %s\n", type, SDL_GetError());
  exit(1);
}

struct Vertex {
  float x;
  float y;
  int angle;
};

struct Vertex 
get_point(struct Vertex base, float radius, float angle) {
  return (struct Vertex) {
    .x = (base.x + (radius * sin(angle * (M_PI/180)))),
    .y = (base.y + (radius * cos(angle * (M_PI/180)))),
    .angle = angle
  };
}

int 
main() {
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

  struct Vertex forward, point_a, point_b;
  
  while (game) {

    /* continuous-response keys */
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    /*
     * Using same algorithm to find our lead point away from the center, 
     * we can use it to find the next point of our center (movement). The
     * radius in this case becomes our speed.
     *
     * We move in the direction of our angle.
     */
    if(keystate[SDL_SCANCODE_W]) {
      center = get_point(center, 1, center.angle);
    }

    if(keystate[SDL_SCANCODE_S]) {
      center = get_point(center, -1, center.angle);
    }

    /*
     * Move the angle of our point left or right.
     */
    if(keystate[SDL_SCANCODE_A]) {
      center.angle = (center.angle + 2) % 360;
    }

    if(keystate[SDL_SCANCODE_D]) {
      center.angle = (center.angle - 2) % 360;
    }

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

    /* Update forward and its points */
    forward = get_point(center, radius, center.angle);
    point_a = get_point(center, radius, center.angle + 135);
    point_b = get_point(center, radius, center.angle - 135);

    SDL_SetRenderDrawColor(render, 0, 0, 0, 255); 
    SDL_RenderClear(render);

    /* Draw Center point in white */
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderDrawPoint(render, center.x, center.y);

    /* Use slope intercept to get line's equation */
    my = (float)(center.y - forward.y);
    mx = (float)(center.x - forward.x);
    m  = (float)(my/mx);
    b  = center.y - (m * center.x);

    /* Find the bounds */
    if (center.x < forward.x) {
      left  = center.x;
      right = forward.x;
    } else {
      left  = forward.x;
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
    SDL_RenderDrawPoint(render, forward.x, forward.y);
    SDL_RenderDrawPoint(render, point_a.x, point_a.y);
    SDL_RenderDrawPoint(render, point_b.x, point_b.y);

    SDL_RenderPresent(render);
  }
  
  SDL_Quit();

  return EXIT_SUCCESS;
}
