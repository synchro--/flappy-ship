#include "elements.hxx"


//Implementation of the objects in elements.hxx
namespace elements {
   Floor::Floor(const char *texture_filename)
   : m_size(100.0f),
     m_height(0.0f),
     m_env(agl::get_env()),
     // repat = true, linear interpolation
     m_tex(m_env.loadTexture(texture_filename, true)) {}

    void render {
      lg::i(__func__, "Rendering floor...");
      m_env.drawFloor(m_tex,  m_size, m_height, 150);
    }

    Floor* get_floor(const char *filename) {
      const static auto TAG = __func__;
      lg::i(TAG, "Loading floor texture from %s", texture_filename);

      static std::unique_ptr<Floor> s_floor(nullptr);
      if(!s_floor) {
        s_floor.reset(new Floor()); //Init
      }

      return s_floor.get();
    }

  Sky::Sky(const char *texture_filename)
  : m_radius(100.0f),
    m_lats(20.0f),
    m_longs(20.0f),
    m_env(agl::get_env()),
    m_tex(m_env.loadTexture(texture_filename, false)) {}

    void render {
      lg::i(__func__, "Rendering Sky...");
      m_env.drawSky(m_tex, m_radius, m_lats, m_longs);
    }

    void set_params(double radius = 100.0,
                       int lats = 20, int longs = 20)
                       {
                         m_radius = radius;
                         m_lats = lats;
                         m_longs = longs;
                       }
    Floor* get_floor(const char *filename) {
      const static auto TAG = __func__;
      lg::i(TAG, "Loading Sky texture from %s", texture_filename);

      static std::unique_ptr<Sky> s_sky(nullptr);
      if(!s_sky) {
        s_sky.reset(new Sky()); //Init
      }

      return s_sky.get();
    }

}
