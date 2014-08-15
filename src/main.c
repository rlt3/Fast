#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "polygon.h"

#define MAX_ASTEROIDS   16
#define ASTEROID_ANGLES  4
#define PLAYER_ANGLES    3

#define ARC4RANDOM_MAX  0x100000000

void
fatal(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
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
  /* Height / width ratio */
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

/* Wrapper for glTranslatef */
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
    for (i = 0; i < PLAYER_ANGLES; i++) {
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
  glBegin(GL_QUADS);
    int j, i;
    for (j = 0; j < MAX_ASTEROIDS; j++) {
      if (asteroids[j] == NULL) { continue; }

      for (i = 0; i < ASTEROID_ANGLES; i++) {
        struct Vertex vertex = vertex_from_angle(*asteroids[j], 
            asteroids[j]->angles[i]);
        glVertex2f(vertex.x, vertex.y);
      }
    }

  glEnd();
}

/* create randomized asteroids to kill our player */
struct Polygon *
construct_asteroid() 
{

  /*
   * TODO: Random angles and placement for our asteroids
   */

  /* allocate space for our asteroid on the heap */
  struct Polygon *p = (struct Polygon*) malloc(sizeof(struct Polygon));

  if (p == NULL) {
    fatal("Out of memory.");
  }


  /* define the asteroid and allocate space for the angles array */
  *p = (struct Polygon) {
    .center = (struct Vertex){ 
      /* 
       * Generate a random number between 0.0f and 1.0f, multiply it
       * by 10 to get a range from 0.0f to 10.0f and then subtract 5.0f
       * to get a final range of -5.0f to 5.0f
       */
      .x = ((((double)arc4random() / ARC4RANDOM_MAX) * 10.0f) - 5.0f),
      .y = 2.0f,
    },
    .angle  = rand() % 91,
    .radius = 1,
    .angles = (float *) malloc(sizeof(float) * ASTEROID_ANGLES)
  };

  if (p->angles == NULL) {
    fatal("Out of memory.");
  }

  /* Get a random number for each quadrant (0..90 and then add the angle) */
  p->angles[0] = rand() % 91;
  p->angles[1] = 90  + (rand() % 91);
  p->angles[2] = 180 + (rand() % 91);
  p->angles[3] = 270 + (rand() % 91);

  return p;
}

/* Free the space of our asteroids */
void
deconstruct_asteroids(struct Polygon *p[])
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (p[i] == NULL) { continue; }

    free(p[i]->angles);
    free(p[i]);
  }
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
  /* seed our random number generator */
  srand(time(NULL));

  int i;

  float angles[PLAYER_ANGLES] = { 0, -135, 135 };

  struct Polygon player = (struct Polygon) {
    .center = (struct Vertex){ 0.0f, -3.0f },
    .angles = angles,
    .angle  = 90,
    .radius = 1
  };

  struct Polygon * asteroids[MAX_ASTEROIDS] = { NULL };

  /* construct all of our asteroids */
  //for (i = 0; i < MAX_ASTEROIDS; i++) {
  for (i = 0; i < 1; i++) {
    asteroids[i] = construct_asteroid();
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

      player.angle = 90;

      /* Movement up and down */
      if (keystate[SDL_SCANCODE_W]) {
        player.center = vertex_shift(player, speed);
      }

      if (keystate[SDL_SCANCODE_S]) {
        player.center = vertex_shift(player, -speed);
      }

      /* Turning left or right */
      if (keystate[SDL_SCANCODE_A]) {
        player.angle = 110;
        player.center.x -= 0.25;
      }

      if (keystate[SDL_SCANCODE_D]) {
        player.angle = 70;
        player.center.x += 0.25;
      }

      /* set the display for drawing */
      set_display(displayRenderer);

      /* Draw everything */
      display_player(displayRenderer, player);
      display_asteroids(displayRenderer, asteroids);

      render(displayRenderer);
    }
  }
  
  SDL_Quit();
  deconstruct_asteroids(asteroids);

  return 0;
}
