#include <iostream>

#include <SDL2/SDL.h>

bool point_inside(SDL_Point p, SDL_Point p1, SDL_Point p2, SDL_Point p3) {
  float alpha = 
    ((float)((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
            ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y)));

  float beta  = 
    ((float)((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
            ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y)));

  float gamma = (float)(1.0f - alpha - beta);

  return (alpha >= 0 && beta >= 0 && gamma >= 0);
}

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
    
    SDL_Point center = {polygon[0].x, polygon[3].y};

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

    float x, y, m, b, my, mx;
    int last;

    /* 
     * Equation of line is always: y = mx+b. X & Y are the coordinate pairs.
     * M is the slope. B is the y-intercept.
     *
     * m = (x2 - x1)/(y2 - y1)
     * b = y - mx
     *
     * When filling the points, start when y of current position equals
     * y in slope intercept form: y = mx - b
     */

    for (i = 0; i < 4; i++) {
      /* Make sure all points get connected, so connect first and last */
      last = (i == 0 ? 3 : i-1);
      
      /* Do top and bottom of slope to keep the syntax more sane */
      my = (float)(polygon[last].y - polygon[i].y);
      mx = (float)(polygon[last].x - polygon[i].x);
      m  = (float)(my/mx);
      b  = polygon[i].y - (m * polygon[i].x);

      /* 
       * For each point x and y, parse the current triangle (2 points plus the
       * center) and see if point lies inside triangle. If it does, draw it.
       * 
       * This could probably be improved by just doing the whole thing all at
       * once rather than in sub triangles.
       */
      for (x = left; x < right; x++) {
        for (y = top; y < bottom; y++) {
          SDL_Point z = {x, y};
          if (point_inside(z, polygon[i], polygon[last], center)) {
            SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
            SDL_RenderDrawPoint(render, x, y);
          }
        }
      }
    }

    SDL_RenderPresent(render);
  }

  SDL_Quit();
  
  return 0;
}
