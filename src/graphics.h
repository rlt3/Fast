#ifndef FAST_GRAPHICS_H
#define FAST_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "polygon.h"

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
};

void Display_InitGL();
int  Display_SetViewport(int width, int height);
int  initialize_graphics(struct Graphics *graphics);

void set_display();
void display_triangle(struct Polygon p);
void display_quads(struct Polygon *quads[], int max);

void render(struct Graphics *graphics);

#endif
