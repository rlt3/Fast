#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

/*
 * Polygon:
 *
 * Contains array of vertices. All vertices in polygon have angles which point
 * towards the center of the polygon.
 *
 * The center of the polygon is a vertex and it's angle determines the direction
 * the polygon moves.
 */

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 512

#define RIGHT         1
#define LEFT         -1
#define NONE          0

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

/* Quadratice bezier curve. Set 2 to 5 for cool stuff */
struct Vertex 
bezier_point(struct Vertex a,
             struct Vertex b,
             struct Vertex c,
             float         t) {
  return (struct Vertex) {
    .x = (1 - t) * (1 - t) * a.x + 2 * (1 - t) * t * b.x + t * t * c.x,
    .y = (1 - t) * (1 - t) * a.y + 2 * (1 - t) * t * b.y + t * t * c.y
    //.x = (1 - t)^2 * a.x + 2 * (1 - t) * t * b.x + t^2 * c.x,
    //.y = (1 - t)^2 * a.y + 2 * (1 - t) * t * b.y + t^2 * c.y
  };
}

struct Vertex 
v_sub(struct Vertex a, struct Vertex b) {
  return (struct Vertex){(a.x - b.x), (a.y - b.y)};
}

int 
x_product(struct Vertex a, struct Vertex b) {
  return ((a.x * b.y) - (a.y * b.x));
}

int 
get_side(struct Vertex a, struct Vertex b) {
  int x = x_product(a, b);

  if (x < 0) {
    return LEFT;
  } else if (x > 0) {
    return RIGHT;
  }

  return NONE;
}

bool 
inside_convex_polygon(struct Vertex point, 
                      struct Vertex vertices[], 
                      int           length) {
  int       previous_side = NONE;
  int       current_side;
  struct Vertex a, b, affine_segment, affine_point;
  
  int n;
  for (n = 0; n < length; n++) {
    a = vertices[n];
    b = vertices[(n + 1) % length];

    affine_segment = v_sub(b, a);
    affine_point   = v_sub(point, b);
    current_side   = get_side(affine_segment, affine_point);

    if (current_side == NONE) {
      /* outside or over an edge */
      return false;
    } else if (previous_side == NONE) { 
      /* First Segment */
      previous_side = current_side;
    } else if (previous_side != current_side) {
      return false;
    }
  }

  return true;
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

  /* Our polygon */
  struct Vertex forward, point_a, point_b;

  /* Helper variables */
  struct Vertex point;
  float t;
  
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

    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    for (t = 0; t <= 1; t = t + 0.01) {
      point = bezier_point(point_a, forward, point_b, t);
      SDL_RenderDrawPoint(render, point.x, point.y);
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
