#include "Coordinate.hpp"

bool Coordinate::operator< (const Coordinate &that) const {
  return (abs(this->x) < abs(that.x) && abs(this->y) < abs(that.y));
}

bool Coordinate::operator> (const Coordinate &that) const {
  return (abs(this->x) > abs(that.x) && abs(this->y) > abs(that.y));
}

bool Coordinate::operator==(const Coordinate &that) const {
  return (this->x == that.x && this->y == that.y);
}

Coordinate& Coordinate::operator+=(const Coordinate &that) {
  this->x += that.x;
  this->y += that.y;
  return *this;
}

Coordinate& Coordinate::operator*=(const Coordinate &that) {
  this->x *= that.x;
  this->y *= that.y;
  return *this;
}

Coordinate& Coordinate::operator-=(const Coordinate &that) {
  this->x -= that.x;
  this->y -= that.y;
  return *this;
}

Coordinate& Coordinate::operator+ (const Coordinate &that) {
  return *this += that;
}

Coordinate& Coordinate::operator* (const Coordinate &that) {
  return *this *= that;
}

Coordinate& Coordinate::operator- (const Coordinate &that) {
  return *this -= that;
}

Coordinate& Coordinate::operator*=(const int &scale) {
  x *= scale;
  y *= scale;
  return *this;
}

Coordinate& Coordinate::operator+=(const int &scale) {
  x += scale;
  y += scale;
  return *this;
}

Coordinate& Coordinate::operator-=(const int &scale) {
  x -= scale;
  y -= scale;
  return *this;
}

Coordinate& Coordinate::operator* (const int &scale) {
  return *this *= scale;
}

Coordinate& Coordinate::operator+ (const int &scale) {
  return *this += scale;
}

Coordinate& Coordinate::operator- (const int &scale) {
  return *this -= scale;
}
