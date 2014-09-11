#ifndef FAST_GRAPHICS_H
#define FAST_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "vertex.h"
#include "SOIL.h"

/* 
 * Handles setting up the window and drawing stuff onto the window .
 * Should probably not reference the polygon struct but oh well.
 *
 * There are lots of comments for setting everything up -- even for simple 
 * things -- because I cannot remember the arcane commands.
 */

struct Graphics {
  SDL_Window       *window;
  SDL_Renderer     *renderer;
  SDL_RendererInfo  renderer_info;

  GLuint            texture;
};

//int LoadTexture(struct Graphics *graphics, const char *filename);

void Display_InitGL();
int  Display_SetViewport(int width, int height);
int  initialize_graphics(struct Graphics *graphics);

void set_display();
//void display_triangle(struct Graphics *graphics, struct Polygon p);
//void display_quads(struct Graphics *graphics, struct Polygon *quads[], int max);

void display_triangle(GLuint texture, float  x, float  y, float **vertices);
void display_quad(GLuint texture, float  x, float  y, float **vertices);

void render(struct Graphics *graphics);

#endif
