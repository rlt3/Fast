#include <stdio.h>
#include "Entity.hpp"

Entity::Entity() 
  : movement(64, 64, 0, 0, 5)
  , moving_(true)
  , frame_(0)
  , state_(0)
  , rate_(150)
  , last_(0)
  , str_(10)
  , hp_(10)
{ }

Entity::Entity(int x, int y, bool moving, uint8_t frame, uint8_t state, 
               uint8_t rate, unsigned last, uint8_t str, uint8_t hp)
  : movement(x, y, 0, 0, 5)
  , moving_(moving)
  , state_(state)
  , rate_(rate)
  , last_(last)
  , str_(str)
  , hp_(hp)
{ }

void Entity::update(unsigned now) {
  movement.step(moving_, now);

  if (now - last_ < rate_) {
    last_  = now;
    frame_ = ++frame_ % 3;
  }
}

void Entity::set_state(uint8_t state) {
  moving_ = false;

  switch(state) {
  case Direction::UP:
  case Direction::RIGHT:
  case Direction::DOWN:
  case Direction::LEFT:
    move(state);
    break;
  case 5: /* attack */
    break;
  case 10: /* keyup */
    moving_ = true;
    break;
  default:
    break;
  }
}

void Entity::attack(int& hp) {
  hp -= attack_power(); 
}

uint8_t Entity::state() {
  return state_;
}

Area Entity::location() {
  struct Coordinate location = movement.current();
  Area area = {location.x, location.y};
  return area;
}

Area Entity::frame() {
  Area area = {0, 0, 32, 32};
  return area;
}

void Entity::move(uint8_t direction) {
  movement.turn(direction);
}

uint8_t Entity::attack_power() {
  return str_ / 2;
}
