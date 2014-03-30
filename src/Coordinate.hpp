#ifndef COORDINATE_HPP
#define COORDINATE_HPP

#include <cstdlib>
#include <SDL2/SDL.h>

struct Coordinate {
  int x;
  int y;

  bool operator< (const Coordinate &) const;
  bool operator> (const Coordinate &) const;
  bool operator==(const Coordinate &) const;

  Coordinate& operator+=(const Coordinate &);
  Coordinate& operator*=(const Coordinate &);
  Coordinate& operator-=(const Coordinate &);
  Coordinate& operator+ (const Coordinate &);
  Coordinate& operator* (const Coordinate &);
  Coordinate& operator- (const Coordinate &);

  Coordinate& operator*=(const int &);
  Coordinate& operator+=(const int &);
  Coordinate& operator-=(const int &);
  Coordinate& operator* (const int &);
  Coordinate& operator+ (const int &);
  Coordinate& operator- (const int &);
};

typedef Coordinate Location;
typedef Coordinate Point;

#endif
