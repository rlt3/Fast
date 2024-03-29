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

GLuint
load_texture(const char *texture)
{
  GLuint tex_num = SOIL_load_OGL_texture(texture,
      SOIL_LOAD_AUTO,
      SOIL_CREATE_NEW_ID,
      SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | 
      SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

  /* check for an error during the load process */
  if( 0 == tex_num ) {
    printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    return -1;
  }

  return tex_num;
}

int
load_textures(struct Graphics *graphics)
{

  graphics->asteroid_texture    = load_texture("textures/asteroid.png");
  graphics->ship_texture        = load_texture("textures/ship.png");
  graphics->main_screen_texture = load_texture("textures/fast.png");
  graphics->collision_texture   = load_texture("textures/collision.png");
  graphics->level_texture       = load_texture("textures/level.png");
  graphics->score_texture       = load_texture("textures/end.png");

  char number_path[15];

  /* load all number textures 0-9 */
  int i;
  for (i = 0; i < 10; i++) {
    sprintf(number_path, "textures/%d.png", i);
    graphics->number_textures[i] = load_texture(number_path);
  }

  return 0;
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

  if (DISPLAY_TEXTURES) {
    if (load_textures(graphics) != 0) {
      return 2;
    }
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

  /* Set the camera center and depth to give us more manageable numbers */
  glTranslatef(0.0f, 0.0f, -10.0f);
}

/* wrapper for our library's render function */
void
render(struct Graphics *graphics)
{
  SDL_RenderPresent(graphics->renderer);
}

void 
display_triangle(GLuint texture, float  x, float  y, float **vertices)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  
  glBegin(GL_TRIANGLES);
    int i;
    for (i = 0; i < 3; i++) {
      switch(i) {
        case 0:
          glTexCoord2f(0.5f, 1.0f);
          break;

        case 1:
          glTexCoord2f(0.0f, 0.0f);
          break;

        case 2:
          glTexCoord2f(1.0f, 0.0f);
          break;
      }
      glVertex2f(vertices[i][X], vertices[i][Y]);
    }
  glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

}

void 
display_quad(GLuint texture, float  x, float  y, float **vertices)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

  glBegin(GL_QUADS);
    int i;
    for (i = 0; i < 4; i++) {
      /* We map counter clockwise starting from 0 degrees. Top-right first. */
      switch(i) {
        case 0:
          glTexCoord2f(1.0f, 1.0f);
          break;

        case 1:
          glTexCoord2f(0.0f, 1.0f);
          break;

        case 2:
          glTexCoord2f(0.0f, 0.0f);
          break;

        case 3:
          glTexCoord2f(1.0f, 0.0f);
          break;
      }
      glVertex2f(vertices[i][X], vertices[i][Y]);
    }
  glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void
cleanup_graphics()
{
  SDL_Quit();
}
