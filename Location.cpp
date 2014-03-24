#include "Location.hpp"
#include <iomanip>

/*
 * Start definitions for class Location
 */

Location::Location() {
  this->x = 0;
  this->y = 0;
}

Location::Location(int x, int y) {
  this->x = x;
  this->y = y;
}

/* 
 * Get a random destination (to simulate AI) with velocity so that all the 
 * speeds of various entities will work with this.
 */
Location Location::new_destination(int velocity) {
  unsigned short int magnitude = 30;
  unsigned short int i = rand() % 5;

  int direction[5][2] = { {0,1}, {-1,0}, {0,0}, {1,0}, {0,-1} };

  Location destination;
  destination.x = this->x + direction[i][0] * (velocity * magnitude);
  destination.y = this->y + direction[i][1] * (velocity * magnitude);

  return destination;
}

/*
 * There are no diagonals in this game. So, we follow the X-axis first towards 
 * a direction, then the Y-axis.
 */
Location Location::get_direction_to() {
  Location direction;

  if(this->x != 0) {
    direction = Location(sign(this->x), 0);
  } else {
    direction = Location(0, sign(this->y));
  }

  return direction;
}

bool Location::is_same(Location other) {
  return (this->x == other.x && this->y == other.y);
}

bool Location::is_nearby(Location other) {
  Location diff = Location::subtract(*this, other);
  return (abs(diff.x) <= 100 && abs(diff.y) <= 100);
}

/* Tests if center of location is half a sprite's size away */
bool Location::is_adjacent(Location other) {
  Location l1_center(this->x + 32, this->y + 32);
  Location l2_center(other.x + 32, other.y + 32);

  Location diff = Location::subtract(l1_center, l2_center);

  return (abs(diff.x) <= 48 && abs(diff.y) <= 48);
}

Area Location::get_world_area(int radius) {
  return this->get_world_area_offset(0, radius);
}

Area Location::get_world_area_offset(int offset, int radius) {
  Area area;
  area.p1 = Location(this->x + offset, this->y + offset);
  area.p2 = Location((this->x  + offset) + radius, (this->y + offset) + radius);
  return area;
}

bool Location::collides_with(Location other) {
  //Area p = this->get_world_area_offset(16, 32);
  //Area r = other.get_world_area_offset(16, 32);
  Area p = this->get_world_area_offset(4, 12);
  Area r = other.get_world_area_offset(4, 12);
  return p.intersects(r);
}

/* static */
Location Location::subtract(Location l1, Location l2) {
  Location location((l1.x - l2.x), (l1.y - l2.y));
  return location;
}

/* static */
Location Location::add(Location l1, Location l2) {
  Location location((l1.x + l2.x), (l1.y + l2.y));
  return location;
}

/* static */
Location Location::multiply(Location l1, Location l2) {
  Location location((l1.x * l2.x), (l1.y * l2.y));
  return location;
}

/* static */
int Location::sign(int val) {
  return (0 < val) - (val < 0);
}

/*
 * Start definitions for class Area
 */

Area::Area() {
  this->p1 = Location(0, 0);
  this->p2 = Location(0, 0);
}

Area::Area(Location p1, Location p2) {
  this->p1 = p1;
  this->p2 = p2;
}

bool Area::intersects(Area other) {
  return ! ( this->p2.y < other.p1.y || 
             this->p1.y > other.p2.y || 
             this->p2.x < other.p1.x || 
             this->p1.x > other.p2.x );
}
