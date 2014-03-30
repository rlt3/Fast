#include "Movement.hpp"

//Movement::Movement()
// : location_(),
// , magnitude_(0)
// , mag_max_(5) 
// , direction_(1) 
// , accelerating_(false)
// , friction_(100) 
// , last_(0)
//{ }

Movement::Movement(int x, int y, int speed)
 : location_{x, y}
 , magnitude_(0)
 , mag_max_(speed) 
 , direction_(1) 
 , accelerating_(false)
 , friction_(100) 
 , last_(0)
{ }

void Movement::update(unsigned now) {
  update_location(now);
}

void Movement::accelerating(bool value) {
  accelerating_ = value;
}

bool Movement::accelerating() {
  return accelerating_;
}

void Movement::speed(std::function<int(int)> f) {
  mag_max_ = f(mag_max_); 
}

int Movement::speed() {
  return magnitude_;
}

void Movement::turn(int direction) {
  direction_ = direction;
}

Location Movement::location() {
  return location_;
}

void Movement::update_location(unsigned now) {
  if ((now - last_) < friction_) return;
  last_ = now;

  magnitude_ = set_magnitude();
  location_  = next();
}

int Movement::set_magnitude() {
  return (oscilate() == -1 || magnitude_ == mag_max_) ? magnitude_ : oscilate();
}

int Movement::oscilate() {
  return (accelerating_) ? magnitude_ + 1 : magnitude_ - 1;
}

Location Movement::next() {
  return next(direction_);
}

Location Movement::next(int direction) {
  return location_ + (Movement::cardinal(direction) * magnitude_);
}

/* static */
Location Movement::cardinal(int direction) {
  switch(direction) {
  case Direction::UP:
    return (Location){0, -1};

  case Direction::RIGHT:
    return (Location){1, 0};

  case Direction::DOWN:
    return (Location){0, 1};

  case Direction::LEFT:
    return (Location){-1, 0};

  default:
    return (Location){0, 0};
  }
}
