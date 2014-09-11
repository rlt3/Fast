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

  graphics->texture = SOIL_load_OGL_texture
  (
    "asteroid.png",
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
  );
  
  /* check for an error during the load process */
  if( 0 == graphics->texture ) {
    printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
  }

  //glGenTextures(1, &graphics->texture);
  //glBindTexture(GL_TEXTURE_2D, graphics->texture);

  //int width, height;
  //unsigned char* image =
  //    SOIL_load_image("ship.bmp", &width, &height, 0, SOIL_LOAD_RGB);

  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
  //              GL_UNSIGNED_BYTE, image);

  //SOIL_free_image_data(image);
 
  // Poor filtering, or ...
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 
  // ... nice trilinear filtering.
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glGenerateMipmap(GL_TEXTURE_2D);

  //if (LoadTexture(graphics, "ship.bmp") != 0) {
  //  return 2;
  //}

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

/* Draw the player's ship */
//void
//display_triangle(struct Graphics *graphics, struct Polygon p)
//{
//  //glEnable(GL_TEXTURE_2D);
//  //glBindTexture(GL_TEXTURE_2D, graphics->texture);
//
//  //glBegin(GL_TRIANGLES);
//  //  glTexCoord2f(0.5f, 1.0f);
//  //  glVertex2f(0.0f, 1.0f);
//
//  //  glTexCoord2f(0.0f, 0.0f);
//  //  glVertex2f(-1.0f, -1.0f);
//
//  //  glTexCoord2f(1.0f, 0.0f);
//  //  glVertex2f(1.0f, -1.0f);
//  //glEnd();
//
//  //glDisable(GL_TEXTURE_2D);
//
//  //glEnable(GL_TEXTURE_2D);
//  //glBindTexture(GL_TEXTURE_2D, graphics->texture);
//  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//
//  glBegin(GL_TRIANGLES);
//    int i;
//    for (i = 0; i < 3; i++) {
//      //switch(i) {
//      //  case 0:
//      //    glTexCoord2f(0.5f, 1.0f);
//      //  case 1:
//      //    glTexCoord2f(0.0f, 0.0f);
//      //  case 2:
//      //    glTexCoord2f(1.0f, 0.0f);
//      //}
//
//      //glTexCoord2f(p.vertices[i].x, p.vertices[i].y);
//      glVertex2f(p.vertices[i].x, p.vertices[i].y);
//    }
//  glEnd();
//  //glDisable(GL_TEXTURE_2D);
//}

void 
display_triangle(GLuint texture, float  x, float  y, float **vertices)
{
  //glEnable(GL_TEXTURE_2D);
  //glBindTexture(GL_TEXTURE_2D, texture);
  glPushMatrix();
  glTranslatef(x, y, -10.0f);

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

  glPopMatrix();
  //glDisable(GL_TEXTURE_2D);
}

/* draw quad from a pointer array */
//void
//display_quads(struct Graphics *graphics, struct Polygon *quads[],
//              int             max)
//{
//  //glBegin(GL_QUADS);
//  //  glTexCoord2f(0.0f, 1.0f);
//  //  glVertex2f(-2.0f, 2.0f);
//
//  //  glTexCoord2f(1.0f, 1.0f);
//  //  glVertex2f(2.0f, 2.0f);
//
//  //  glTexCoord2f(1.0f, 0.0f);
//  //  glVertex2f(2.0f, -2.0f);
//
//  //  glTexCoord2f(0.0f, 0.0f);
//  //  glVertex2f(-2.0f, -2.0f);
//  //glEnd();
//
//  //glDisable(GL_TEXTURE_2D);
//
//  glEnable(GL_TEXTURE_2D);
//  glBindTexture(GL_TEXTURE_2D, graphics->texture);
//  int   i, j;
//  float x, y;
//    for (i = 0; i < max; i++) {
//      if (quads[i] == NULL) { continue; }
////
////      x = quads[i]->center.x;
////      y = quads[i]->center.y;
////
////      /* push our current setup so we can pop and go back */
////      glPushMatrix();
////
//     // glTranslatef(x, y, -10.0f);
//
//     // glBegin(GL_QUADS);
//     //   glTexCoord2f(0.0f, 0.0f);
//     //   glVertex2f(-x, -y);
//
//     //   glTexCoord2f(1.0f, 0.0f);
//     //   glVertex2f(x, -y);
//
//     //   glTexCoord2f(1.0f, 1.0f);
//     //   glVertex2f(x, y);
//
//     //   glTexCoord2f(0.0f, 1.0f);
//     //   glVertex2f(-x, y);
//
//      for (j = 0; j < 4; j++) {
//        switch(j) {
//          case 0:
//            glTexCoord2f(1.0f, 1.0f);
//            break;
//
//          case 1:
//            glTexCoord2f(0.0f, 1.0f);
//            break;
//
//          case 2:
//            glTexCoord2f(0.0f, 0.0f);
//            break;
//            
//          case 3:
//            glTexCoord2f(1.0f, 0.0f);
//            break;
//        }
//
//        glVertex2f(quads[i]->vertices[j].x, quads[i]->vertices[j].y);
//      }
//      
//      glEnd();
//
//      //glPopMatrix();
//    }
//
//  glDisable(GL_TEXTURE_2D);
//}

void 
display_quad(GLuint texture, float  x, float  y, float **vertices)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  /* save current opengl settings */
  glPushMatrix();

  /* then translate so we have an easy reference for drawing + texturing */
  glTranslatef(x, y, -10.0f);

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

  /* go back to the saved settings */
  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
}

void
cleanup_graphics()
{
  SDL_Quit();
}
