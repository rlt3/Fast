#include "graphics.h"

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

int
initialize_graphics(struct Graphics *graphics)
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL, 
      &graphics->window, &graphics->renderer);
  
  SDL_GetRendererInfo(graphics->renderer, &graphics->renderer_info);

  /*TODO: Check that we have OpenGL */
  if ((graphics->renderer_info.flags & SDL_RENDERER_ACCELERATED) == 0 || 
      (graphics->renderer_info.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
    return 1;
  }

  Display_InitGL();
  Display_SetViewport(800, 600);

  return 0;
}

/* setup the display to be drawn on */
void 
set_display()
{
  /* Set the background black */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Clear The Screen And The Depth Buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* Set the center and depth to give us more manageable numbers */
  glTranslatef(0.0f, 0.0f, -10.0f);
}

/* wrapper for our library's render function */
void
render(struct Graphics *graphics)
{
  SDL_RenderPresent(graphics->renderer);
}

/* Draw the player's ship */
void
display_triangle(struct Polygon p)
{
  glBegin(GL_TRIANGLES);
    int i;
    for (i = 0; i < 3; i++) {
      glVertex2f(p.vertices[i].x, p.vertices[i].y);
    }
  glEnd();
}

/* draw quad from a pointer array */
void
display_quads(struct Polygon *quads[],
              int             max)
{
  int i, j;
  glBegin(GL_QUADS);
    for (i = 0; i < max; i++) {
      if (quads[i] == NULL) { continue; }

      for (j = 0; j < 4; j++) {
        glVertex2f(quads[i]->vertices[j].x, quads[i]->vertices[j].y);
      }
    }
  glEnd();
}

void
cleanup_graphics()
{
  SDL_Quit();
}
