
#include "elements.h"
#include <cmath>

// Implementation of the objects in elements.h
namespace elements {

/*
 * Floor
 */
Floor::Floor(const char *texture_filename)
    : m_size(100.0f), m_height(0.0f), m_env(agl::get_env()),
      // repat = true, linear interpolation
      m_tex(m_env.loadTexture(texture_filename, true, false)) {}

void Floor::render() {
  // lg::i(__func__, "Rendering floor...");
  m_env.drawFloor(m_tex, m_size, m_height, 150);
}

Floor *get_floor(const char *texture_filename) {
  const static auto TAG = __func__;
  lg::i(TAG, "Loading floor texture from %s", texture_filename);

  static std::unique_ptr<Floor> s_floor(nullptr);
  if (!s_floor) {
    s_floor.reset(new Floor(texture_filename)); // Init
  }

  return s_floor.get();
}

/*
 * Sky
 */

Sky::Sky(const char *texture_filename)
    : m_radius(150.0f), m_lats(20.0f), m_longs(20.0f), m_env(agl::get_env()),
      m_tex(m_env.loadTexture(texture_filename, false)) {}

void Sky::render() {
  // lg::i(__func__, "Rendering Sky...");
  m_env.drawSky(m_tex, m_radius, m_lats, m_longs);
}

void Sky::set_params(double radius, int lats, int longs) {
  m_radius = radius;
  m_lats = lats;
  m_longs = longs;
}

Sky *get_sky(const char *texture_filename) {
  const static auto TAG = __func__;
  lg::i(TAG, "Loading Sky texture from %s", texture_filename);

  static std::unique_ptr<Sky> s_sky(nullptr);
  if (!s_sky) {
    s_sky.reset(new Sky(texture_filename)); // Init
  }

  return s_sky.get();
 }

 /*  
  * Ring. See elements::Ring 
  * 
  */

  Ring::Ring(float x, float y, float z, float angle, bool flight_mode
  : m_ship_old_z(INFINITY
    {
    m_px = x;
    m_py = y; 
    m_pz = z; 
    m_angle = angle;
    m_3D_FLIGHT = flight_mode; 
    }
 
} // namespace elements
