#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <functional>
#include "Direction.hpp"
#include "Coordinate.hpp"

class Movement {
public:
  //Movement();
  Movement(int x, int y, int speed);

  /* Set the constructor to default for aggregate construction above */
  Movement() = default;

  /* Give object the time */
  void update(unsigned now);

  /* Set if accelerating */
  void accelerating(bool value);

  /* Get accelerating */
  bool accelerating();

  /* Set speed */
  void speed(std::function<int(int)> f);

  /* Get speed */
  int speed();

  /* Turn direction */
  void turn(int direction);

  /* Get location */
  Location location();

  /* Get cardinal direction from direction */
  Location cardinal(int direction);

protected:
  /* Update based on time and friction (delay) */
  void update_location(unsigned now);

  /* See if our magnitude is at its maxes before setting */
  int set_magnitude();

  /* Oscilate magnitude based on acceleration (true or false) */
  int oscilate();

  /* Get next location from our direction_ member */
  Location next();

  /* Get the next location in any given direction */
  Location next(int direction);

private:
  Location location_ = {0, 0};

  int      magnitude_;
  int      mag_max_;
  int      direction_;

  bool     accelerating_;

  int      friction_;
  unsigned last_;

  /*
   * Give the Movement a function which handles its max movement. It would be a
   * lambda that simply always returns the given direction at first. But, it can
   * be updated to check for bounds by another object.
   */

  /*
   * Create a Vector class which holds the direction and magnitude. So, instead
   * of doing if (accelerating) { } or seeing if the magnitude is over or under
   * the max or seeing which direction is up, just use polymorphism.
   */

};

#endif
