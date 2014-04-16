/*
 * gcc -o test open.c -framework SDL2 -framework OpenGL
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

  /* The order of these matter just like they did before ! */
  //glBegin(GL_POLYGON);
  //  int i;
  //  for (i = 0; i < N_POINTS; i++) {
  //    /* 
  //     * Get the x, y from the angle of the vertices ! This way we do not have
  //     * to keep state except for where the object would be centered. We do need
  //     * to know the original construction, but that is handled by the angles.
  //     */
  //    glVertex3f(
  //      sin((p.vertices[i] + p.angle) * (M_PI/180)),
  //      cos((p.vertices[i] + p.angle) * (M_PI/180)),
  //      0.0f
  //    );
  //  }
  //glEnd();
  
  /* Make a circle out of triangles */
  glBegin(GL_TRIANGLE_FAN);
    /* center */
    glVertex3f(0.0f, 0.0f, 0.0f);

    /* Every 10 degrees, plot a point of a triangle */
    int i;
    for (i = 0; i <= 360; i = i + 10) {
      glVertex3f(
        sin(i * (M_PI/180)),
        cos(i * (M_PI/180)),
        0.0f
      );
    }
  glEnd();

  glTranslatef(-10.0f, 0.0f, 0.0f);

  /* Does using Fan mean I have to have a right triangle? No. */
  glBegin(GL_TRIANGLE_FAN);
    /* center */
    glVertex3f(0.0f, 0.0f, 0.0f);

    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -0.5f, 0.0f);
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
  
  SDL_Delay(5000);
  SDL_Quit();
  
  return 0;
}
