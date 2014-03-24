#ifndef DEARTH_COORDINATE_HPP
#define DEARTH_COORDINATE_HPP

#include <cstdlib>

struct Coordinate {
  int x;
  int y;

  Coordinate() : x(0), y(0) { }
  Coordinate(int x, int y) : x(x), y(y) { }

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

#endif
