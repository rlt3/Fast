#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>

#include "polygon.h"
#include "asteroid.h"

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

/* Draw the player's ship */
void
display_player(SDL_Renderer* displayRenderer, 
               struct Polygon player)
{
  glBegin(GL_TRIANGLES);
    int i;
    for (i = 0; i < player.sides; i++) {
      glVertex2f(player.vertices[i].x, player.vertices[i].y);
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
        glVertex2f(asteroids[j]->vertices[i].x, asteroids[j]->vertices[i].y);
      }
    }

  glEnd();
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

  struct Vertex vertices[PLAYER_ANGLES] = { 
    (struct Vertex) { 0, 0,    0 },
    (struct Vertex) { 0, 0, -135 },
    (struct Vertex) { 0, 0,  135 },
  };

  struct Polygon player = (struct Polygon) {
    .center   = (struct Vertex){ 0.0f, -3.0f, 90 },
    .vertices = vertices,
    .radius   = 1,
    .sides    = PLAYER_ANGLES
  };

  struct Polygon * asteroids[MAX_ASTEROIDS] = { NULL };

  /*
   *  TODO: 
   *    - Scale our asteroid count by time
   *    - Collision on asteroids ends game
   */

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
      speed = speed + 0.1f;
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

      player.center.angle = 90;

      /* Movement up and down */
      if (keystate[SDL_SCANCODE_W]) {
        player.center = vertex_shift(player.center, speed);
      }

      if (keystate[SDL_SCANCODE_S]) {
        player.center = vertex_shift(player.center, -speed);
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

      handle_asteroids(asteroids, player, speed);
      update_vertices(&player);

      set_display(displayRenderer);

      display_player(displayRenderer, player);
      display_asteroids(displayRenderer, asteroids);

      render(displayRenderer);
    }
  }

  //printf("area: %f\n\n", polygon_area(player));
  //printf("area: %f\n",   polygon_area(*asteroids[0]));

  //printf("asteroid center inside? %s\n", 
  //  (point_inside_polygon(asteroids[0]->center, *asteroids[0]) ? "true" : "false"));

  //printf("player center inside? %s\n", 
  //  (point_inside_polygon(player.center, *asteroids[0]) ? "true" : "false"));
  
  SDL_Quit();
  deconstruct_asteroids(asteroids);

  return 0;
}
