#include "Vector.hpp"

Vector::Vector() : 
  location_()
  , angle_(0)
  , magnitude_(0)
  , speed_(5)
  , friction_(100)
  , last_(0)
{ }

Vector::Vector(int x, int y, uint8_t angle, uint8_t magnitude, uint8_t speed)
  : location_(x, y)
  , angle_(angle)
  , magnitude_(magnitude)
  , speed_(speed) 
  , friction_(100) 
  , last_(0)
{ }

void Vector::step(bool accelerating, unsigned now) {
  oscilate_magnitude(accelerating, now);
  location_ = next_step();
}

struct Coordinate Vector::next_step() {
  return next_step(angle_);
}

struct Coordinate Vector::next_step(uint8_t angle) {
  return (location_ + (Vector::get_direction(angle) * magnitude_));
}

void Vector::turn(uint8_t angle) {
  angle_ = angle;
}

void Vector::speed_up() {
  speed_ *= 2;
}

void Vector::slow_down() {
  speed_ /= 2;
}

struct Coordinate Vector::current() {
  return location_;
}

struct Coordinate Vector::direction() {
  return Vector::get_direction(angle_);
}

/* static */
struct Coordinate Vector::direction_towards(struct Coordinate destination) {
  return Coordinate(0, 0);
}

void Vector::oscilate_magnitude(bool accelerating, uint32_t now) {
  if (now - last_ < friction_) return;
  last_ = now;
  
  if(!accelerating && magnitude_ <= speed_)
    magnitude_ += (magnitude_ == speed_) ? 0 : 1;
  else
    magnitude_ -= (magnitude_ == 0) ? 0 : 1;
}

/* static */
struct Coordinate Vector::get_direction(uint8_t angle) {
  switch(angle) {
  case Direction::UP :
    return Coordinate(0, -1);

  case Direction::RIGHT :
    return Coordinate(1, 0);

  case Direction::DOWN :
    return Coordinate(0, 1);

  case Direction::LEFT :
    return Coordinate(-1, 0);

  default:
    return Coordinate(0, 0);
  }
}
