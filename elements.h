#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#include "types.h"

#include "agl.h"
#include "log.h"

/*
 * elementsh describes the different objects (elements) of the game
 * Floor, Sky, ..., it will be expanded as the game gets more complex
 * and new elements gets added.
 */

namespace elements {

// utility function, returns the string related to the motion
const std::string motion_to_str(spaceship::Motion mt);

/*
 * The floor.
 * Construcor loads the texture and stores it in m_tex
 * Render function draw this tex
 * get_floor returns a singleton instance. This behaviour can be changed
 * in case we want to expand the game and get more instances of floors (or
 * planes).
 */

class Floor {
private:
  float m_size, m_height;
  agl::Env &m_env;  // reference to env, needed in the constructor
  agl::TexID m_tex; // floor texture

  // construct the floor loading the texture
  Floor(const char *texture_filename);

public:
  // friend function to load the texture and create a singleton
  friend Floor *get_floor(const char *filename);

  void render();
};

// get singleton instance of floor
Floor *get_floor(const char *filename);

// The Sky. Same mechanism as above
class Sky {
private:
  agl::Env &m_env;
  agl::TexID m_tex;
  double m_radius;
  int m_lats, m_longs;

  // construct the floor loading the texture
  Sky(const char *texture_filename);

public:
  // friend function to load the texture and create a singleton
  friend Sky *get_sky(const char *filename);

  void render();

  // accessors
  void set_params(double radius = 100.0, int lats = 20, int longs = 20);

  inline decltype(m_radius) radius() { return m_radius; }
};

// get singleton instance of sky
Sky *get_sky(const char *filename);

/*
 * RING class.
 * Rings are torus polygons that must be crossed to win the game.
 * Whenever a ring is crossed the player gets bonus time for the next one.
 * Whenever a ring is crossed a next one will be spawned somewhere (random
 * coords)
 *
 * When the player crossed all the required rings, a special Final Gate will be
 * triggered. See class Gate.
 */

class Ring {
private:
  float m_px, m_py, m_pz; // coords
  float m_ship_old_z;     // the previous ship position wrt ring ref frame
  float m_angle;          // wrt Y-axis
  bool m_3D_FLIGHT; // when true rings can have positive y-coord, thus be in the
                    // "sky"
  bool m_triggered;
  agl::Env &m_env; // env reference

public:
  // static members
  // colors for when the ring is triggered or not
  static const agl::Color TRIGGERED;
  static const agl::Color NOT_TRIGGERED;
  // view UP vector
  static const agl::Vec3 s_viewUP;
  // radius values
  static const float s_r;
  static const float s_R;

  Ring(float x, float y, float z, bool m_3D_FLIGHT = false, float angle = 30.0);

  void render();

  // check if the new ship position has crossed the ring
  void checkCrossing(float x, float z);
  // same but for flight mode
  void checkCrossing(float x, float y, float z);

  // accessors
  inline float x() { return m_px; }
  inline float y() { return m_py; }
  inline float z() { return m_pz; }
  inline bool isTriggered() { return m_triggered; }
};

/*
 * Introducing: the BAD cube.
 * It's a simple wireframe cube, but if the spaceship touches it the player gets
 * a penalty time.
 * Besides, the spaceship starts flickering to indicate that something bad
 * happened.
 */

class BadCube {
private:
  float m_px, m_py, m_pz; // coords
  float m_ship_old_z;     // the previous ship position wrt ring ref frame
  float m_angle;          // wrt Y-axis
  bool
      m_3D_FLIGHT; // when true, cubes can have positive y-coord, thus be in the
                   // "sky"
                   // bool m_triggered;

  agl::Env &m_env; // env reference

public:
  // static members
  // view UP vector
  static const agl::Vec3 s_viewUP;
  // radius values
  static const float side;

  BadCube(float x, float y, float z, bool m_3D_FLIGHT = false,
          float angle = 30.0);

  void render();

  // check if the new ship position has crossed the ring
  bool checkCrossing(float x, float z);
  // same but for flight mode
  bool checkCrossing(float x, float y, float z);

  // accessors
  inline float x() { return m_px; }
  inline float y() { return m_py; }
  inline float z() { return m_pz; }
};

/*
 * The Final Door to Life
 *
 * This is the door Truman will have to cross to gain his freedom.
 */

class Door {

private:
  std::unique_ptr<agl::Mesh> m_mesh;
  agl::TexID m_tex;
  float m_px, m_py, m_pz;             // coords
  float m_scaleX, m_scaleY, m_scaleZ; // scaling factors
  float m_ship_old_z; // the previous ship position wrt ring ref frame
  float m_angle;      // wrt Y-axis

  agl::Env &m_env; // env reference

  // private cons, see get_door
  Door(const char *mesh_filename, const char *texture_filename);

public:
  friend std::unique_ptr<Door> get_door(const char *mesh_filename,
                                        const char *texture_filename);

  // static members
  // view UP vector
  static const agl::Vec3 s_viewUP;
  // radius values to check crossing
  static const float side;

  void render();

  // check if the new ship position has crossed the ring
  bool checkCrossing(float x, float z);

  // accessors
  inline float x() { return m_px; }
  inline float y() { return m_py; }
  inline float z() { return m_pz; }
};

std::unique_ptr<Door> get_door(const char *mesh_filename,
                               const char *texture_filename);

} // namespace elements

#endif //_ELEMENTS_H_
