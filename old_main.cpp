#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL_image/SDL_image.h>

#include "Direction.hpp"
#include "Entity.hpp"

#define SCREENX           640
#define SCREENY           512

SDL_Surface * load_sheet(const char filename[]) {
  SDL_Surface *temp = IMG_Load(filename);

  SDL_SetColorKey(temp, 
      (SDL_SRCCOLORKEY|SDL_RLEACCEL), 
      SDL_MapRGB(temp->format, 0, 0, 0));

  if (temp == NULL) {
    printf("Error: %s\n", IMG_GetError());
    exit(1);
  }

  SDL_Surface *sprite = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  if (sprite == NULL) {
    printf("Failed to convert image to native format %s\n", filename);
    exit(1);
  }

  return sprite;
}

void load_window(SDL_Surface **screen) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_WM_SetCaption("Fast", "Fast");
  SDL_EnableKeyRepeat(70, 70);
  //SDL_SetVideoMode(SCREENX, SCREENY, 0, SDL_OPENGL | SDL_FULLSCREEN);
  SDL_SetVideoMode(SCREENX, SCREENY, 0, 0);

  atexit(SDL_Quit);

  *screen = SDL_GetVideoSurface();
}

int main(int argc, char **argv) {
  static SDL_Surface *screen;
  static SDL_Surface *font;
  static Entity player;

  load_window(&screen);
  static SDL_Surface *spritesheet = load_sheet("player.png");

  unsigned last_time = SDL_GetTicks();

  SDL_Event event;

  bool on = true;
  while (on) {

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
          case SDLK_ESCAPE: case SDL_QUIT:
            on = false;
            break;

          case SDLK_w: case SDLK_UP: case SDLK_k:
            player.set_state(Direction::UP);
            break;

          case SDLK_a: case SDLK_LEFT: case SDLK_h:
            player.set_state(Direction::LEFT);
            break;

          case SDLK_s: case SDLK_DOWN: case SDLK_j:
            player.set_state(Direction::DOWN);
            break;

          case SDLK_d: case SDLK_RIGHT: case SDLK_l:
            player.set_state(Direction::RIGHT);
            break;

          case SDLK_LSHIFT: case SDLK_RSHIFT:
            player.movement.speed_up();
            break;

          default:
            break;
          }
          break;

        case SDL_KEYUP:
          player.set_state(10);
          switch (event.key.keysym.sym) {
          case SDLK_LSHIFT: case SDLK_RSHIFT:
            player.movement.slow_down();
            break;
          }
          break;
      }
    }

    player.update(SDL_GetTicks());

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xCC, 0xCC, 0xCC));

/*
 * The magnitude of the Entity affects the framerate. So, have each entity's 
 * frame() function handle itself and return just a flat number for whatever
 * frame it is supposed to be on.
 *
 * Since we need to derive where on the spritesheet we need to get the image, 
 * we can use the type of Entity as an offset. So, there's no reason to do
 * graphics math inside the Entity function. Just handle it's frame, which is
 * basically which "foot" it's currently on.
 */

    SDL_Rect frame    = player.frame();
    SDL_Rect location = player.location();
    SDL_BlitSurface(spritesheet, &frame, screen, &location);

    SDL_Flip(screen);
  }

  SDL_FreeSurface(spritesheet);
  return EXIT_SUCCESS;
}
