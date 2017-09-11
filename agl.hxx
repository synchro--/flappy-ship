#ifndef _AGL_HXX_
#define _AGL_HXX_

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <GL/gl.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game.hxx"
#include "log.hxx"
#include "types.hxx"

/*
* AGL: Abstract Graphic Library
* The purpose is to create an abstract layer on the top of OpenGL in order to
* simplify the usage of all the graphic functions in the project
*
* Note: methods have camelCase usually.
*       Small ones like accessors, however, have _underscore_ case.
*/

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

// Point3 is defined out of clarity, to distinguish a point from
// a vector ;
using Point3 = Vec3

// struct representing the vertex of a mesh.
struct Vertex {
  Vec3 point;
  Normal3 normal;

  Vertex(const Vec3 &v = Vec3());

  void render(bool send_normal = false) const;
};

// Enables double buffering
void enable_double_buffering() {
  lg::i(__func__, "enabling double buffer");

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

// Enables the depth buffer at depth 'depth'
void enable_zbuffer(size_t depth) {
  lg::i(__func__, "setting up Z-buffer of depth = %zu bits", depth);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
}

/* The Env class represents the Environment of the game.
   It handles all the main components of the scene and all the callbacks
   associated with the commands.
*/
class Env final {

private:
  Env(); // constructs the Environment, intializing all the variables.

  float m_view_alpha, m_view_beta;
  float m_eye_dist;
  int m_screenH, m_screenW;
  int m_camera_type;
  float m_fps;     // fps value in the last interval
  float m_fps_now; // fps currently drawn
  int m_step;      // number of steps of Physics currently done
  bool m_wireframe, m_envmap, m_headlight, m_shadow;
  uint32_t m_last_time;

  /* Callbacks variables:
  *  they will be the handler for keys, windows events and rendering.
  *  The actual callback function will vary according to the current
  *  state of the game.
  *  For example, if we are on Menù, the rendering callback will be different
  *  wrt when we are actually playing.
  */
  std::function<void()> m_action_handler, m_render_handler,
      m_window_event_handler;
  std::function<void(Key)> m_key_up_handler, m_key_down_handler;

public:
  // Friends can touch your private parts.
  friend Env &get_env();

  // destructor takes care of closing the SDL libraries
  virtual ~Env();

  // drawing functions
  void drawFloor(TexID texbind, float sz, float height, size_t num_quads);
  void drawSky(TexID texbind, double radius, int lats, int longs);
  void drawSphere(double r, int lats, int longs);

  // Returns the current FPS.
  inline decltype(m_fps) fps() { return m_fps; }

  using TexID = GLuint;

  // Load texture from an image and return bool if success, should be changed to
  // return a texture ID --i.e. unsigned integer
  TexID loadTexture(const char *filename, bool repeat = false,
                    bool nearest = false);

  // Accepts a lambda to be performed between push and pop
  // Saves time and ensures the matrix will be popped after
  // pushing
  void mat_scope(const std::function<void()> callback);
  void redraw();
  void setCoordToPixel();

  // Setters for all the callbacks
  // Default: empty
  void set_action(decltype(m_action_handler) actions = [] {});
  void set_keydown_handler(decltype(m_key_down_handler) onkeydown = [](Key) {});
  void set_keyup_handler(decltype(m_key_up_handler) onkeyup = [](Key) {});
  void set_render(decltype(m_render_handler) render = [] {});
  void set_winevent_handler(decltype(m_window_event_handler) onwinev = [] {});

  // Model and Projection matrix setup
  void setup_model();
  void setup_persp(float width, float height);

  // texture drawing helper function
  void textureDrawing(TexID texbind, std::function<void()> callback,
                      bool gen_coordinates = true);
};

// return singleton instance
Env &get_env();
}

#endif
