/*
 * Locations in this game are where the sprite is drawn: starting at the 
 * top-left corner. Knowing this, we have to make some adjustments and 
 * translations for the game to behave as someone might expect it to.
 *
 * A lot of times we adjust a location to be in the center of the sprite it 
 * represents. I call this it's World Location.
 *
 * Areas are rectangles and consist of two locations. The first location is 
 * the top-left corner of the rectangle and the second is the bottom right.
 *
 * Knowing these two locations of a rectangle allows us to see if things 
 * intersect.
 */

#ifndef ROGUE_LOCATION_HPP
#define ROGUE_LOCATION_HPP

#include <cstdlib>
#include <time.h>

class Area;

class Location {
public:
  int x;
  int y;

  Location();
  Location(int x, int y);

  Location new_destination(int velocity);
  Location get_direction_to();

  bool is_same(Location other);
  bool is_nearby(Location other);
  bool is_adjacent(Location other);
  bool collides_with(Location other);

  Area get_world_area(int radius);
  Area get_world_area_offset(int offset, int radius);

  static Location translate(Location location);
  static Location subtract(Location l1, Location l2);
  static Location add(Location l1, Location l2);
  static Location multiply(Location l1, Location l2);
  static int sign(int value);
};

class Area {
public:
  Location p1;
  Location p2;

  Area();
  Area(Location p1, Location p2);

  bool intersects(Area other);
};

#endif
