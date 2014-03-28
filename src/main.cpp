#include <stdio.h>
#include <SDL2/SDL.h>

#include "Direction.hpp"
#include "Movement.hpp"

int main() {
  Movement vector;

  while (1) {
    vector.update(SDL_GetTicks());
    vector.accelerating(true);

    //vector.turn([&](direction) {
    //  case SDLK_w: case SDLK_UP: case SDLK_k:
    //    player.set_state(Direction::UP);
    //    break;

    //  case SDLK_a: case SDLK_LEFT: case SDLK_h:
    //    player.set_state(Direction::LEFT);
    //    break;

    //  case SDLK_s: case SDLK_DOWN: case SDLK_j:
    //    player.set_state(Direction::DOWN);
    //    break;

    //  case SDLK_d: case SDLK_RIGHT: case SDLK_l:
    //    player.set_state(Direction::RIGHT);
    //    break;
    //});

    Location loc = vector.location();

    if (loc.x > 50) {
      break;
    }
  }

  return 0;
}
