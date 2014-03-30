#ifndef VECTOR_HPP
#define VECTOR_HPP

typedef Coordinate Location;

class Vector {
public:
  Vector& operator* (const Location &);

protected:

private:
  int  magnitude_;
  int  direction_;
  bool accelerating_;

};
