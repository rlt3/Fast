#include "Polygon.hpp"

Point Polygon::direction[] = {
  {1, -1}, /* top right */
  {1,  0}, /* right     */
  {1,  1}, /* bot right */
  {-1, 1}, /* bot left  */
  {-1, 0}, /* left      */
  {-1,-1}  /* top left  */
};

Polygon::Polygon() : center_(200, 200, 5), radius_(25) { 
  create_points(); 
}

Polygon::Polygon(int x, int y) : center_(x, y, 5), radius_(25) { 
  create_points(); 
}

Point* Polygon::vertices() {
  return points_;
}

int Polygon::points() {
  return num_points_;
}

void Polygon::create_points() {
  for (int i = 0; i < 6; i++) {
    points_[i] = center_.location() + (direction[i] * radius_);
  }
}
