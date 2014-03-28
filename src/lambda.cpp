#include <functional>
#include <stdio.h>

#include <SDL2/SDL.h>

struct Location {
  int x;
  int y;

  Location() : x(0), y(0) { }
  Location(int x, int y) : x(x), y(y) { }

  Location& operator+=(const int &scale) {
    x += scale;
    y += scale;
    return *this;
  }

  Location& operator+ (const int &scale) {
    return *this += scale;
  }
};

class Movement {
public:
  Movement()
  : location_(0, 0)
  , magnitude_(0)
  , accelerating_(false)
  , max_speed_(5) 
  , friction_(100) 
  , last_(0)
  { set_movement(); }
 
  Movement(int x, int y, int magnitude, int speed)
  : location_(x, y)
  , magnitude_(magnitude)
  , accelerating_(false)
  , max_speed_(speed) 
  , friction_(100) 
  , last_(0)
  { set_movement(); }

  /* Update based on time */
  //void msg(unsigned now) {
  //template<typename Functor>
  void msg(std::function<unsigned long(int)> f) {
    update(f(magnitude_));
  }

  /* Set accelerating */
  void msg(std::function<bool(void)> f) {
    accelerating_ = f();
  }

  /* How to set it up so once can pass a literal message?
   * movement.msg([]() { accelerating = false; }
   */
  //template<typename Functor>
  //void msg(Functor f) {
  //  f();
  //}

  /* Set speed */
  //void msg(std::function<int(int)> f) {
  //  max_speed_ = f(max_speed_); 
  //}

  /* Get Location */
  struct Location msg(std::function<struct Location(struct Location)> f) {
    return f(location_);
  }

protected:

  void update(unsigned now) {
    if ((now - last_) < friction_) return;
    last_ = now;

    velocity_  = accelerating_ ? set_acceleration() : set_deceleration();
    magnitude_ = velocity_(magnitude_, max_speed_);
    location_  = location_ + magnitude_;
  }

  void set_movement() {
    //movement_ = [](struct Location loc) { return loc + magnitude_ };
  }

  std::function<int(int, int)> set_deceleration() {
    return [](int mag, int max) { return (mag == 0) ? mag : mag - 1; };
  }

  std::function<int(int, int)> set_acceleration() {
    return [](int mag, int max) { return (mag == max) ? mag : mag + 1; };
  }

private:
  struct Location location_;

  int magnitude_;
  int max_speed_;
  int friction_;
  bool accelerating_;

  unsigned last_;

  std::function<int(int, int)> velocity_;
  std::function<int(int)> movement_;
};

int main() {
  Movement vector;

  vector.msg([]() { return true; });

  while (1) {
    vector.msg([&](int magnitude) {
      if (magnitude > 4) {
        vector.msg([]() { return false; });
      }

      return SDL_GetTicks();
    });

    struct Location loc = vector.msg([](struct Location loc) { 
      return loc;
    });

    if (loc.x > 20) {
      break;
    }
  }

  return 0;
}
