/*
 * gcc -o test open.c -framework SDL2 -framework OpenGL
 *
 * clang -o test open.c -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -framework OpenGL
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#define DIMENSION 3
#define N_POINTS  3

struct Vertex {
  float x;
  float y;
};

struct Polygon {
  struct Vertex center;
  float angles[3];

  int    angle;
  int    radius;
};

struct Vertex
create_vertex(float x, float y, float radius, float angle)
{
  return (struct Vertex) {
    .x = (x + (radius * cos(angle * (M_PI/180)))),
    .y = (y + (radius * sin(angle * (M_PI/180)))),
  };
}

struct Vertex
vertex_shift(struct Polygon p, float amount)
{
  return create_vertex(p.center.x, p.center.y, amount, p.angle);
}

struct Vertex
vertex_from_angle(struct Polygon p, float angle)
{
  return create_vertex(p.center.x, p.center.y, p.radius, (p.angle + angle));
}

void 
Display_InitGL()
{
  /* Enable smooth shading */
  glShadeModel(GL_SMOOTH);

  /* Set the background black */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Depth buffer setup */
  glClearDepth(1.0f);

  /* Enables Depth Testing */
  glEnable(GL_DEPTH_TEST);

  /* The Type Of Depth Test To Do */
  glDepthFunc(GL_LEQUAL);

  /* Really Nice Perspective Calculations */
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

/* function to reset our viewport after a window resize */
int 
Display_SetViewport(int width, int height)
{
  /* Height / width ration */
  GLfloat ratio;

  /* Protect against a divide by zero */
  if (height == 0) {
      height = 1;
  }

  ratio = (GLfloat)width / (GLfloat)height;

  /* Setup our viewport. */
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  /* change to the projection matrix and set our viewing volume. */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* Set our perspective */
  gluPerspective(45.0f, ratio, 0.1f, 100.0f);

  /* Make sure we're chaning the model view and not the projection */
  glMatrixMode(GL_MODELVIEW);

  /* Reset The View */
  glLoadIdentity();

  return 1;
}

/*
 * Wrapper for glTranslatef
 */
void
glTranslatefv(GLfloat *point) {
  glTranslatef(point[0], point[1], point[2]);
}

void 
Display_Render(SDL_Renderer* displayRenderer, struct Polygon polygon)
{
  /* Set the background black */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Clear The Screen And The Depth Buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* Set the center */
  glTranslatef(0.0f, 0.0f, -6.0f);

  glBegin(GL_TRIANGLES);

    /*
     * The polygon itself holds an angle. Each vertex is at a set angle
     * relative to the polygon. So, using some trigonometry we can use those
     * angles to find where they fix on a coordinate plane.
     */

    int i;
    for (i = 0; i < 3; i++) {
      struct Vertex vertex = vertex_from_angle(polygon, polygon.angles[i]);
      glVertex2f(vertex.x, vertex.y);
    }

  glEnd();

  SDL_RenderPresent(displayRenderer);
}

int
main(int argc, char *argv[])
{
  struct Polygon polygon = (struct Polygon) {
    .center = (struct Vertex){ 0.0f, 0.0f },
    .angles = { 0, -135, 135 },
    .angle  = 90,
    .radius = 1
  };

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window       *displayWindow;
  SDL_Renderer     *displayRenderer;
  SDL_RendererInfo  displayRendererInfo;
  SDL_Event         event;

  SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL, 
      &displayWindow, &displayRenderer);

  SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);

  /*TODO: Check that we have OpenGL */
  if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 || 
      (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
      /*TODO: Handle this. We have no render surface and not accelerated. */
  }
  
  Display_InitGL();
  Display_SetViewport(800, 600);

  bool   game  = true;
  float  speed = 0.1;
  Uint32 last = SDL_GetTicks();

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
      if (keystate[SDL_SCANCODE_W])
        polygon.center = vertex_shift(polygon, speed);

      if (keystate[SDL_SCANCODE_S])
        polygon.center = vertex_shift(polygon, -speed);

      /* Turning left or right */
      if (keystate[SDL_SCANCODE_A])
        polygon.angle = (polygon.angle + 2) % 360;

      if (keystate[SDL_SCANCODE_D])
        polygon.angle = (polygon.angle - 2) % 360;

      Display_Render(displayRenderer, polygon);
    }

  }
  
  SDL_Quit();
  
  return 0;
}
