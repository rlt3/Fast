#ifndef DEARTH_ENTITY_HPP
#define DEARTH_ENTITY_HPP

#include <stdint.h>
#include <SDL/SDL.h>
#include "Vector.hpp"

/*
 * Since Vector handles every bit of movement for the entity, we can have 
 * states that overlap with the directions.
 *
 * Changing player's state from one thing to another will not affect it's
 * movement or location, but, basically, which sprite gets drawn. Will 
 * probably need to change `idle' to what it really means (walking or not
 * walking).
 *
 * So, an entity could be walking right and then attack, changing its state.
 * It's direction never actually got changed, but its state did. But an
 * entity does not move when it is attacking. So, set idle to true (or 
 * whatever it will be) and then update the state so that, ultimately, it
 * affects the frame that gets drawn on the screen.
 *
 * This way of doing things implies a single access point: change_state from
 * the old class. The entity will turn itself, or attack, or whatever based
 * on the input given -- but it will determine itself how.
 */

typedef SDL_Rect Area;

class Entity {
public:
  Vector movement;
  
  Entity();
  Entity(int, int, bool, uint8_t, uint8_t, uint8_t, unsigned, uint8_t, uint8_t);

  /* Update based on change in game ticks and last updated tick */
  void update(unsigned dt);

  /* Get current state */
  uint8_t state();

  /* Set state based on player input or game input (AI) */
  void set_state(uint8_t state);

  /* Accept a reference to an HP value and damage it */
  void attack(int&);

  /* Get current location as an Area */
  Area location();

  /* Get current frame as an Area */
  Area frame();

protected:
  void move(uint8_t direction);
  uint8_t attack_power();

private:
  bool    moving_;
  uint8_t frame_;
  uint8_t state_;

  uint8_t str_;
  uint8_t hp_;

  uint8_t  rate_;
  unsigned last_;

};

#endif
