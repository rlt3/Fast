#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "polygon.h"
#include "asteroid.h"
#include "star.h"

#define PLAYER_ANGLES    3

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

/* setup the display to be drawn on */
void 
set_display(SDL_Renderer* displayRenderer)
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
render(SDL_Renderer* displayRenderer) 
{
  SDL_RenderPresent(displayRenderer);
}

/* Draw the player's ship */
void
display_player(SDL_Renderer* displayRenderer, struct Polygon player)
{
  glBegin(GL_TRIANGLES);
    int i;
    for (i = 0; i < player.sides; i++) {
      glVertex2f(player.vertices[i].x, player.vertices[i].y);
    }
  glEnd();
}

/* pause until a button is hit */
void
pause(SDL_Event *event)
{
  while (1) {
    while (SDL_PollEvent(event)){
      switch (event->type) {
        case SDL_KEYDOWN:
          return;
      }
    }
  }
}

bool
player_collision(struct Polygon *asteroids[], 
                 struct Polygon  player)
{
  int i;
  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroids[i] == NULL) { continue; }

    if (triangle_intersects_polygon(player, *asteroids[i])) {
      return true;
    }
  }
  return false;
}

/* draw quad from a pointer array */
void
display_quads(SDL_Renderer   *displayRenderer, 
              struct Polygon *quads[],
              int             max)
{
  int i, j;
  glBegin(GL_QUADS);
    for (i = 0; i < max; i++) {
      if (quads[i] == NULL) { continue; }

      for (j = 0; j < quads[i]->sides; j++) {
        glVertex2f(quads[i]->vertices[j].x, quads[i]->vertices[j].y);
      }
    }
  glEnd();
}

void
set_game(struct Polygon *player, struct Polygon *asteroids[], float *speed)
{
  player->center.x =  0.0f;
  player->center.y = -3.0f;
  player->center.angle = 90;

  deconstruct_polygon_array(asteroids, MAX_ASTEROIDS);
  
  *speed = 0.01f;
}

int
main(int argc, char *argv[])
{
  /* seed our random number generators */
  srand(time(NULL));
  srand48(time(NULL));

  struct Polygon * stars[MAX_STARS]         = { NULL };
  struct Polygon * asteroids[MAX_ASTEROIDS] = { NULL };

  /* make all the stars now so they show up immediately */
  construct_all_stars(stars);

  struct Vertex vertices[PLAYER_ANGLES] = { 
    (struct Vertex) { 0, 0,    0 },
    (struct Vertex) { 0, 0, -135 },
    (struct Vertex) { 0, 0,  135 },
  };

  struct Polygon player = (struct Polygon) {
    .center   = (struct Vertex){ 0.0f, -3.0f, 90 },
    .vertices = vertices,
    .radius   = 0.75,
    .sides    = PLAYER_ANGLES
  };

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
  float  speed = 0.01f;

  Uint32 last_time  = SDL_GetTicks();
  Uint32 last_speed = last_time;

  while (game) {

    /* Every 10 seconds make the player faster */
    if (SDL_GetTicks() - last_speed > 1000) {
      last_speed = SDL_GetTicks();
      speed = speed + 0.01f;
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

      /* reset to 90 degrees when player isn't turning */
      player.center.angle = 90;

      /* Movement up and down */
      if (keystate[SDL_SCANCODE_W]) {
        player.center = vertex_shift(player.center, 0.1f);
      }

      if (keystate[SDL_SCANCODE_S]) {
        player.center = vertex_shift(player.center, -0.1f);
      }

      /* Tilt the player left and move left */
      if (keystate[SDL_SCANCODE_A]) {
        player.center.angle = 110;
        player.center.x -= speed + 0.25;
      }

      /* Tilt the player right and move right */
      if (keystate[SDL_SCANCODE_D]) {
        player.center.angle = 70;
        player.center.x += speed + 0.25;
      }

      handle_asteroids(asteroids, speed);
      handle_stars(stars, speed);
      update_vertices(&player);

      set_display(displayRenderer);

      display_player(displayRenderer, player);
      display_quads(displayRenderer, asteroids, MAX_ASTEROIDS);
      display_quads(displayRenderer,     stars, MAX_STARS);

      render(displayRenderer);

      /* do collision after rendering so the player can visually see it */
      if (player_collision(asteroids, player)) {
        pause(&event);
        set_game(&player, asteroids, &speed);
      }
    }
  }
  
  SDL_Quit();
  deconstruct_polygon_array(asteroids, MAX_ASTEROIDS);
  deconstruct_polygon_array(stars,     MAX_STARS);

  return 0;
}
