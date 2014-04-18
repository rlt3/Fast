/*
 * gcc -o test open.c -framework SDL2 -framework OpenGL
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

struct Polygon {
  float center[DIMENSION];
  float vertices[N_POINTS];
  int   angle;
  int   radius;
};

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
Display_Render(SDL_Renderer* displayRenderer, struct Polygon p)
{
  /* Set the background black */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Clear The Screen And The Depth Buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* 
   * 1.0f represents the entire width or height of screen. Going ``into'' the
   * screen (negative z index) offsets 1.0f by that much. In this case it is
   * 1.0f/30.0f
   *
   * OpenGL uses all 4 quadrants. The center is 0, 0; This is the center of
   * the polygon we will draw.
   */
  glTranslatefv(&p.center[0]);

  /* Draw a square */
  glBegin(GL_TRIANGLE_FAN);
    /* center */
    glVertex3f(0.0f, 0.0f, 0.0f);

    /* Top right corner */
    glVertex3f(1.5f, 1.0f, 0.0f);
    //glVertex3f(1.0f, 1.0f, 0.0f);
    //glVertex3f(1.0f, 1.0f, 0.0f);

    /* bottom right corner */
    glVertex3f(1.5f, -1.0f, 0.0f);
    //glVertex3f(1.0f, -1.0f, 0.0f);
    //glVertex3f(1.0f, -1.0f, 0.0f);

    /* bottom left corner */
    glVertex3f(-1.5f, -1.0f, 0.0f);
    //glVertex3f(-1.0f, -1.0f, 0.0f);
    //glVertex3f(-1.0f, -1.0f, 0.0f);

    /* top left corner */
    glVertex3f(-1.5f, 1.0f, 0.0f);
    //glVertex3f(-1.0f, 1.0f, 0.0f);
    //glVertex3f(-1.0f, 1.0f, 0.0f);

    /* connect top left to top right */
    glVertex3f(1.5f, 1.0f, 0.0f);
  glEnd();

  glTranslatef(-10.0f, 0.0f, 0.0f);

  /* Draw rounded square */
  glBegin(GL_TRIANGLE_FAN);
    /* center */
    glVertex3f(0.0f, 0.0f, 0.0f);

    float radius = 1;

    int   i; 
    float x, y;
    float firstx, firsty;

    for (i = 20; i <= 60; i = i + 5) {
      x = radius * cos(i * (M_PI/180));
      y = radius * sin(i * (M_PI/180));
      glVertex3f(x, y, 0.0f);
    }

    for (i = 120; i <= 160; i = i + 5) {
      x = radius * cos(i * (M_PI/180));
      y = radius * sin(i * (M_PI/180));
      glVertex3f(x, y, 0.0f);
    }

    for (i = 200; i <= 240; i = i + 5) {
      x = radius * cos(i * (M_PI/180));
      y = radius * sin(i * (M_PI/180));
      glVertex3f(x, y, 0.0f);
    }

    for (i = 300; i <= 340; i = i + 5) {
      x = radius * cos(i * (M_PI/180));
      y = radius * sin(i * (M_PI/180));
      glVertex3f(x, y, 0.0f);
    }

    x = radius * cos(20 * (M_PI/180));
    y = radius * sin(20 * (M_PI/180));
    glVertex3f(x, y, 0.0f);
  glEnd();


  SDL_RenderPresent(displayRenderer);
}

int
main(int argc, char *argv[])
{
  struct Polygon polygon = (struct Polygon) {
    .center   = { 0.0f, 0.0f, -30.0f },
    .vertices = { 0, 135, -135 },
    .angle    = 90,
    .radius   = 50
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
  Display_Render(displayRenderer, polygon);

  bool game = true;

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
  }
  
  SDL_Quit();
  
  return 0;
}
