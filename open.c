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

struct Polygon {
  float center[DIMENSION];
  float vertices[N_POINTS];
  int   angle;
  int   radius;
};

float
quad_bezier(float a, float b, float c, float t)
{
  return (1 - t) * (1 - t) * a + 2 * (1 - t) * t * b + t * t * c;
}

float
cubic_bezier(float a, float b, float c, float d, float t)
{
  return pow((1-t), 3) * a +
         3 * pow((1 - t), 2) * t * b +
         3 * (1 - t) * pow(t, 2) * c +
         pow(t, 3) * d;
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

  /* 
   * 1.0f represents the entire width or height of screen. Going ``into'' the
   * screen (negative z index) offsets 1.0f by that much. In this case it is
   * 1.0f/30.0f
   *
   * OpenGL uses all 4 quadrants. The center is 0, 0; This is the center of
   * the polygon we will draw.
   */
  glTranslatef(0.0f, 0.0f, -5.0f);

  /* Draw rounded square */
  glBegin(GL_TRIANGLE_FAN);

    /* center */
    glVertex3f(0.0f, 0.0f, 0.0f);

    int   i;
    float t;

    /* 
     * Using bezier curves, we can control the look of our square just using
     * the points as handles. If all points in a curve lie on the same axis
     * (x or y), then those points will get a curve that is straight. Otherwise,
     * they have a curve. By adjusting the individual points, we can make curves
     * for the square.
     */
    float p[6][2] = {
      {   1.0f,   1.0f },
      {   1.0f,   0.0f },
      {   1.0f,  -1.0f },
      {  -1.0f,  -1.0f },
      {  -2.5f,   0.0f },
      {  -1.0f,   1.0f }

      //{   1.0f,   1.0f },
      //{   1.0f,  -1.0f },
      //{  -1.0f,  -1.0f },
      //{  -1.0f,   1.0f }
    };

    //for (t = 0.0f; t <= 1.1f; t = t + 0.1f) {
    //  glVertex2f(
    //    cubic_bezier(p[0][0], p[1][0], p[2][0], p[3][0], t),
    //    cubic_bezier(p[0][1], p[1][1], p[2][1], p[3][1], t)
    //  );
    //}

    /* Till length, increment by quadratic bezier points (3) */
    for (i = 0; i < 6; i = i + 3) {
      for (t = 0.0f; t <= 1.1f; t = t + 0.1f) {
        glVertex2f(
          quad_bezier(p[i][0], p[i+1][0], p[i+2][0], t),
          quad_bezier(p[i][1], p[i+1][1], p[i+2][1], t)
        );
      }
    }

    /* Connect last point back to the first */
    glVertex2f(p[0][0], p[0][1]);

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
