#ifndef _AGL_HXX_
#define _AGL_HXX_

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <GL/gl.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game.hxx"
#include "types.hxx"
#include "log.hxx"

/*AGL: Abstract Graphic Library
The purpose is to create an abstract layer on the top of OpenGL in order to
simplify the usage of all the graphic functions in the project*/

namespace agl {

// represents a RGBA color.
struct Color {
  float r, g, b, a;

  Color(float r = 0, float g = 0, float b = 0, float a = 1);
};

extern const Color WHITE, BLACK, RED, GREEN, YELLOW;

// a 2D horizontal vector (point)
struct Vec2 {
  float x, z;
};

// a 3D vector (and point)
struct Vec3 {
  float x, y, z;

  float modulo() const;

  inline void gl_translate() { glTranslatef(x, y, z); }

  Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f);

  Vec3 normalize() const;

  Vec3 operator-() const;

  Vec3 &operator+=(const Vec3 &other);

  Vec3 operator+(const Vec3 &other) const;
  Vec3 operator-(const Vec3 &other) const;
  Vec3 operator/(float f) const;
  // redefine % operator to define cross-product
  Vec3 operator%(const Vec3 &a) const;
};

// a vector that represents a Normal, meant to be rendered as such.
struct Normal3 : public Vec3 {
  Normal3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
  Normal3(const Vec3 &vec3);

  void render() const;
}

// Point3 will be used out of clarity, to distinguish between points and vectors
using Point3 = Vec3;

// struct representing the vertex of a mesh.
struct Vertex {
  Vec3 point;
  Normal3 normal;

  Vertex(const Vec3 &v = Vec3());

  void render(bool send_normal = false) const;
};


/* The Env class represent the Environment of the game.
   It handles all the main components of the scene and all the callbacks
   associated with the commands.
*/
class Env final {

private:
  float m_view_aplha, m_view_beta;
  float m_eye_dist;
  int m_screenH, m_screenW;
  int m_camera_type;
  float m_fps;     // fps value in the last interval
  float m_fps_now; // fps currently drawn
  int m_step;      // number of steps of Physics currently done
  bool m_wireframe, m_envmap, m_headlight, m_shadow;
  unsigned int m_last_time;

  // callbacks variables
  std::function<void()> m_actionf, m_renderf, m_onwinevf;
  std::function<void(Key)> m_onkeyupf, m_onkeydownf;

public:
  //Friends can touch your private parts.
  friend Env &get_env();

  // destructor takes care of closing the SDL libraries
  virtual ~Env();
  // accepts a lambda to be performed between push and pop
  // Saves time and helps against bugs ensuring the matrix will be popped after
  // pushing
  void Env::mat_scope(const std::function<void()> callback);

  // load texture from an image and return bool if success
  bool Env::LoadTexture(int textbind, char *filename);
  void Env::SetCoordToPixel(Game gs);

  // drawing functions
  void Env::drawSphere(double r, int lats, int longs);
  void Env::drawSky();
  void Env::drawFloor();
  void Env::drawAxis();
  void Env::redraw();

  void Env::setup_model();
  void Env::setup_persp(float width, float height);

  // Returns the current FPS.
  inline decltype(m_fps) fps() { return m_fps; }

  // Setters for all the callbacks
  // Default: empty
  void Env::set_action(decltype(m_actionf) actions = [] {});
  void Env::set_onkeydown(decltype(m_onkeydownf) onkeydown = [](Key) {});
  void Env::set_onkeyup(decltype(m_onkeyupf) onkeyup = [](Key) {});
  void Env::set_onwindowevent(decltype(m_onwinevf) onwinev = [] {});
  void Env::set_render(decltype(m_renderf) render = [] {});
};

// return singleton instance
Env &get_env();
}

#endif
