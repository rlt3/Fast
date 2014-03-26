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

  int once = 0;

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

    float center_x;
    float center_y;

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
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

    center_x = polygon[0].x;
    center_y = polygon[3].y;

    SDL_RenderDrawPoint(render, center_x, center_y);

    /* Set color to blue */
    SDL_SetRenderDrawColor(render, 0, 0, 255, 255);

    /* Draw the bounding box */
    SDL_Point bounds[2] = { {left, top}, {right, bottom} };
    SDL_RenderDrawPoints(render, bounds, 2);

    /* Set color to red */
    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);

    float x,  y;            /* Graph coordinates & polygon coordinates */
    float m,  b,  my,  mx;  /* Current  Slope intercept variables */
    int   last;
    int   next;

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

    for (i = 0; i < 1; i++) {
      /* Make sure all points get connected, so connect first and last */
      last = (i == 0 ? 3 : i-1);
      
      /* Do top and bottom of slope to keep the syntax more sane */
      my = (polygon[last].y - polygon[i].y);
      mx = (polygon[last].x - polygon[i].x);
      m  = (float)(my/mx);
      b  = polygon[i].y - (m * polygon[i].x);

      if (once < 4) {
        printf("%d. Now [(%d, %d); (%d, %d)] b: %.2f, m: %.2f\n", i,
            polygon[last].x, polygon[last].y,
            polygon[i].x, polygon[i].y,
            b, m);
        printf("center: (%f, %f)\n", center_x, center_y);
        once++;
      }

      /**
       * We are filling our shape by splitting it into triangles. First we draw
       * the line from points a -> b, where a and b are just i and i-1. Then for
       * every point at x, get the y value using the slope-intercept form. Draw
       * the point if it doesn't exceed the bounds of our shape.
       *
       * Then, to fill our triangle, we need to know the center point of the
       * shape. The center point is the common coordinate comprised of x, y
       * values of each pair. All of our triangles are right triangles.
       *
       * So, for (A.x, A.y) and (B.x, B.y) the third coordinate might be
       * (A.x, B.y) if it falls inside the bounds of the shape. If all the pairs
       * of coordinates share this common point, it is deemed the center of the
       * shape.
       *
       * For each x point (going across) we find the corresponding y for that
       * line. We then start at y and move downwards until we hit the limit for
       * that specific triangle. Repeat this for x across until we have filled 
       * in our triangle.
       *
       * Another, I think, more simpler way is to just get the opposite slope
       * value and start filling in until y has reached the opposite slope for
       * that X.
       */

      int oy;
      /**
       * Instead of left, maybe leftmost point of the two coordinate pairs and
       * goes until rightmost pair?
       */
      for (x = left; x < center_x; x++) { 
        y  = (m*x) + b;
        oy = -(m*x) + b;

        if (y > top && y < bottom) { /* top starts at 0 here */
          SDL_RenderDrawPoint(render, x, y);
        }

        for (y; y < oy; y++) {
          SDL_RenderDrawPoint(render, x, y);
        }
      }
    }

    SDL_RenderPresent(render);
  }

  SDL_Quit();
  
  return 0;
}
