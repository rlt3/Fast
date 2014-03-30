#ifndef POLYGON_HPP
#define POLYGON_HPP

#include "Movement.hpp"

class Polygon {
public:
  Polygon();
  Polygon(int x, int y);

  Point* vertices();
  int    points();

protected:
  void create_points();

private:
  static const int num_points_ = 6;
  static Point     direction[num_points_]; 

  Point    points_[num_points_];
  Movement center_;
  int      radius_;

};

#endif
