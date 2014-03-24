/*
 * In the spirit of DRY, the Vector class takes full advanage of using a struct
 * for it's current location. The Coordinate struct overloads many of the 
 * operators for specific Linear Algebra for the game.
 *
 * Having these operators be attached specifically to points on a plane makes
 * sense because these coordinates may be used elsewhere not necessarily in 
 * the context of a Vector: bliting sprites to a specific location, determining
 * attack and hit boxes for combat, and collision.
 */
#ifndef DEARTH_VECTOR_HPP
#define DEARTH_VECTOR_HPP

#include <stdint.h>
#include "Direction.hpp"
#include "Coordinate.hpp"

class Vector {
public:
  Vector();
  Vector(int x, int y, uint8_t angle, uint8_t magnitude, uint8_t speed);

  /* 
   * The Vector steps forward, changing its location based on angle, 
   * magnitude, and acceleration 
   */
  void step(bool accelerating, unsigned now);

  /* Get coordinates at the location of the next step */
  struct Coordinate next_step();

  /* Get coordinates at location of next step in proposed direction */
  struct Coordinate next_step(uint8_t angle);

  /* Turn towards the direction */
  void turn(uint8_t angle);

  /* Increase magnitude to double its max speed or slow it down to half */
  void speed_up();
  void slow_down();

  /* Get coordinates of Vector's current location */
  struct Coordinate current();

  /* Get direction as a coordinate */
  struct Coordinate direction();

  /* Get the next direction as a coordinate towards the destination */
  static struct Coordinate direction_towards(struct Coordinate destination);

protected:
  void oscilate_magnitude(bool accelerating, uint32_t now);
  static struct Coordinate get_direction(uint8_t angle);

private:
  struct Coordinate location_;

  uint8_t angle_;
  uint8_t magnitude_;
  uint8_t speed_;
  uint8_t friction_;

  unsigned last_;
};

#endif
