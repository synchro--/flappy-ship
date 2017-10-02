
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

  Ring::Ring(float x, float y, float z, float angle, bool flight_mode)
  : m_ship_old_z(INFINITY), m_triggered(false), m_env(agl::get_env())
    {
      m_px = x;
      m_py = y;
      m_pz = z;
      m_angle = angle;
      m_3D_FLIGHT = flight_mode;
    }

  // initaliazing static members of Ring class
  // colors for when the ring is triggered or not
  const agl::Color Ring::TRIGGERED = {1.0f, .86f, .35f, .7f};
  const agl::Color Ring::NOT_TRIGGERED = {.2f, .80f, .2f, .7f};
  //view UP vector
  const  agl::Vec3 Ring::s_viewUP = agl::Vec3(0.0, 1.0, 0.0);
  //radius values
  const  float Ring::s_r = 0.3; // inner radius
  const  float Ring::s_R = 2.5; // outer radius

  void Ring::render() {
      m_env.mat_scope([&] {
        m_env.translate(m_px, m_py, m_pz);
        m_env.rotate(m_angle, s_viewUP);
        // set the proper color if triggered
        m_env.setColor(m_triggered ? TRIGGERED : NOT_TRIGGERED);

        if(m_env.isBlending()) {
          // maybe move this to Env helper function
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

          m_env.drawTorus(s_r, s_R);

          glDisable(GL_BLEND);
        }
        else {
          m_env.drawTorus(s_r, s_R);
       }

      });
    }


    void Ring::checkCrossing(float x, float z) {
      //if the ring has already been crossed, nothing to do
      if(!m_triggered) {
        bool first_call = m_ship_old_z == INFINITY;

        // get distance wrt to ring center
        x-= m_px;
        z-= m_pz;

        float cos_phi = cosf(m_angle * M_PI / 180.0f);
        float sin_phi = sinf(m_angle * M_PI / 180.0f);

        // project coords of the ship to ring reference frame
        float x_ring = x*cos_phi - z*sin_phi;
        float z_ring = x*sin_phi + z*cos_phi;

        // X: is it inside the circle diameter?
        bool check_X = (x_ring < 2*s_R) && (x_ring > -2*s_R);
        // Z: sign changed? Then crossing happened
        bool check_Z = (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

        if(!first_call && check_Z && check_X) {
          m_triggered = true;
        }

        m_ship_old_z = z_ring;
      }
    }

  // as above but checking on all 3Dimesionson for flappy flight mode
  void Ring::checkCrossing(float x, float y, float z) {
     //if the ring has already been crossed, nothing to do
      if (!m_triggered)
      {
        bool first_call = m_ship_old_z == INFINITY;

        // get distance wrt to ring center
        x -= m_px;
        y -= m_py;
        z -= m_pz;

        float cos_phi = cosf(m_angle * M_PI / 180.0f);
        float sin_phi = sinf(m_angle * M_PI / 180.0f);

        // project coords of the ship to ring reference frame
        float x_ring = x * cos_phi - z * sin_phi;
        float y_ring = y;
        float z_ring = x * sin_phi + z * cos_phi;

        // X: is it inside the circle diameter?
        bool check_X = (x_ring < 2 * s_R) && (x_ring > -2 * s_R);
        bool check_Y = (y_ring < 2 * s_R) && (y_ring > -2 * s_R);
       // Z: sign changed? Then crossing happened
        bool check_Z = (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

        if (!first_call && check_Z && check_Y && check_X)
        {
          m_triggered = true;
        }

        m_ship_old_z = z_ring;
       }
    }

} // namespace elements
