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

#define RIGHT  1
#define LEFT  -1
#define NONE   0

SDL_Point v_sub(SDL_Point a, SDL_Point b) {
  return (SDL_Point){(a.x - b.x), (a.y - b.y)};
}

int x_product(SDL_Point a, SDL_Point b) {
  return ((a.x * b.y) - (a.y * b.x));
}

int get_side(SDL_Point a, SDL_Point b) {
  int x = x_product(a, b);

  if (x < 0) {
    return LEFT;
  } else if (x > 0) {
    return RIGHT;
  }

  return NONE;
}

bool inside_convex_polygon(SDL_Point point, SDL_Point vertices[], int length) {
  int       previous_side = NONE;
  int       current_side;
  SDL_Point a, b, affine_segment, affine_point;
  
  int n;
  for (n = 0; n < length; n++) {
    a = vertices[n];
    b = vertices[(n + 1) % length];

    affine_segment = v_sub(b, a);
    affine_point   = v_sub(point, b);
    current_side   = get_side(affine_segment, affine_point);

    if (current_side == NONE) {
      return false; /* outside or over an edge */
    } else if (previous_side == NONE) { /* First Segment */
      previous_side = current_side;
    } else if (previous_side != current_side) {
      return false;
    }
  }

  return true;
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
    int i, x, y;
    
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

    for (x = left; x < right; x++) {
      for (y = top; y < bottom; y++) {
        SDL_Point z = {x, y};
        if (inside_convex_polygon(z, polygon, 4)) {
          SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
          SDL_RenderDrawPoint(render, x, y);
        }
      }
    }

    SDL_RenderPresent(render);
  }

  SDL_Quit();
  
  return 0;
}
