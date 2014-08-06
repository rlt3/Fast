#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "polygon.h"

#define DIMENSION 3
#define N_POINTS  3

#define MAX_ASTEROIDS 16

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

/* Draw the player's ship */
void
display_player(SDL_Renderer* displayRenderer, 
               struct Polygon player)
{
  glBegin(GL_TRIANGLES);

    /*
     * The polygon itself holds an angle. Each vertex is at a set angle
     * relative to the polygon. So, using some trigonometry we can use those
     * angles to find where they fix on a coordinate plane.
     */

    int i;
    for (i = 0; i < 3; i++) {
      struct Vertex vertex = vertex_from_angle(player, player.angles[i]);
      glVertex2f(vertex.x, vertex.y);
    }

  glEnd();
}

/* Draw our asteroids */
void
display_asteroids(SDL_Renderer* displayRenderer, 
                  struct Polygon* asteroids[MAX_ASTEROIDS])
{
  glBegin(GL_TRIANGLES);
    int j, i;
    for (j = 0; j <= MAX_ASTEROIDS; j++) {
      if (asteroids[j] == NULL) { continue; }

      for (i = 0; i < 3; i++) {
        struct Vertex vertex = vertex_from_angle(*asteroids[j], 
            asteroids[j]->angles[i]);
        glVertex2f(vertex.x, vertex.y);
      }
    }

  glEnd();
}

/* setup the display to be drawn on */
void 
set_display(SDL_Renderer* displayRenderer)
{
  /* Set the background black */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Clear The Screen And The Depth Buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* Set the center */
  glTranslatef(0.0f, 0.0f, -10.0f);
}

/* wrapper for our library's render function */
void
render(SDL_Renderer* displayRenderer) 
{
  SDL_RenderPresent(displayRenderer);
}

int
main(int argc, char *argv[])
{
  int i;

  struct Polygon polygon = (struct Polygon) {
    .center = (struct Vertex){ 0.0f, -3.0f },
    .angles = { 0, -135, 135 },
    .angle  = 90,
    .radius = 1
  };

  struct Polygon * asteroids[MAX_ASTEROIDS] = { NULL };

  /* allocate space for our asteroids */
  for (i = 0; i <= MAX_ASTEROIDS; i++) {
    asteroids[i] = (struct Polygon*) malloc(sizeof(struct Polygon));

    *asteroids[i] = (struct Polygon) {
      .center = (struct Vertex){ -0.2f, 0.0f },
      .angles = { 0, 90, -193 },
      .angle  = 0,
      .radius = 1
    };
  }

  /* Initiate our SDL library, window, and render, and declare our variables */ 
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
  float  speed = 0.1f;

  Uint32 last_time  = SDL_GetTicks();
  Uint32 last_speed = last_time;

  while (game) {

    /* Every 10 seconds make the player faster */
    if (SDL_GetTicks() - last_speed > 10000) {
      last_speed = SDL_GetTicks();
      speed = speed + 0.15f;
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

    /* around 24 frames a second */
    if (SDL_GetTicks() - last_time > 40) {
      last_time = SDL_GetTicks();

      const Uint8 *keystate = SDL_GetKeyboardState(NULL);

      polygon.angle = 90;

      /* Movement up and down */
      if (keystate[SDL_SCANCODE_W]) {
        polygon.center = vertex_shift(polygon, speed);
      }

      if (keystate[SDL_SCANCODE_S]) {
        polygon.center = vertex_shift(polygon, -speed);
      }

      /* Turning left or right */
      if (keystate[SDL_SCANCODE_A]) {
        polygon.angle = 110;
        polygon.center.x -= 0.25;
      }

      if (keystate[SDL_SCANCODE_D]) {
        polygon.angle = 70;
        polygon.center.x += 0.25;
      }

      /* set the display for drawing */
      set_display(displayRenderer);

      /* Draw everything */
      display_player(displayRenderer, polygon);
      display_asteroids(displayRenderer, asteroids);

      render(displayRenderer);
    }

  }
  
  SDL_Quit();

  /* Free the space of our asteroids */
  for (i = 0; i <= MAX_ASTEROIDS; i++) {
    free(asteroids[i]);
  }
  
  return 0;
}
