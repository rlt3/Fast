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
    SDL_Point polygon[4] = { {300, 100}, {350, 150}, {300, 200}, {200, 150} };
    SDL_RenderDrawPoints(render, polygon, 4);

    int top    = polygon[0].y;
    int right  = polygon[0].x;
    int bottom = polygon[0].y;
    int left   = polygon[0].x;
    int i; 

    /* Find outer most points for all cardinal directions */
    for (i = 1; i < 4; i++) {
      if (polygon[i].x < left) {
        left = polygon[i].x;
      }

      if (polygon[i].x > right) {
        right = polygon[i].x;
      }

      if (polygon[i].y < top) {
        top = polygon[i].y;
      }

      if (polygon[i].y > bottom) {
        bottom = polygon[i].y;
      }
    }

    /* Set color to blue */
    SDL_SetRenderDrawColor(render, 0, 0, 255, 255);

    /* Draw the bounding box */
    SDL_Point bounds[2] = { {left, top}, {right, bottom} };
    SDL_RenderDrawPoints(render, bounds, 2);

    /* Set color to red */
    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);

    float x, y, m, b;
    for (i = 1; i < 4; i++) {
      m = (polygon[i-1].x - polygon[i].x)/(polygon[i-1].y - polygon[i].y);
      b = polygon[i].y - (m * polygon[i].x);

      for (x = left; x < right; x++) {
        y = (m*x) + b;
        if (y > top && y < bottom) { /* top starts at 0 here */
          SDL_RenderDrawPoint(render, x, y);
        }
      }
    }

    /*
     * When filling the points, start when y of current position equals
     * y in slope intercept form: y = mx - b
     */

    //SDL_Point origin = {300, 100};
    //SDL_Point ending = {350, 150};

    //float m = (ending.x - origin.x)/(ending.y - origin.y);
    //float b = origin.y - (m * origin.x);

    //for (x = left; x < right; x++) {
    //  y = (m*x) + b;
    //  if (y > top) { /* top starts at 0 here */
    //    SDL_RenderDrawPoint(render, x, y);
    //  }
    //}

    /* 
     * Equation of line is always: y = mx+b. X & Y are the coordinate pairs.
     * M is the slope. B is the y-intercept.
     *
     * m = (x2 - x1)/(y2 - y1)
     * b = y - mx
     */

    SDL_RenderPresent(render);
  }
  //printf("y = %.2fx + %f\n", m, b);
  //printf("(%d, %d), (%d, %d)\n", left, top, right, bottom);

  SDL_Quit();
  
  return 0;
}
