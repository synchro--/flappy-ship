#ifndef _AGL_H_
#define _AGL_H_

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "log.h"
#include "types.h"

/*
 * AGL: Abstract Graphic Library
 * The purpose is to create an abstract layer on the top of OpenGL in order to
 * simplify the usage of all the graphic functions in the project
 *
 * Note: methods have camelCase usually.
 *       Small ones like accessors, however, have _underscore_ case.
 */

namespace agl {

// a 2D Point (or horizontal vector)
struct Point2 {
  float x, z;
};

// a 3D vector (and point)
struct Point3 {
  float x, y, z;

  Point3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
  // Point3();

  inline void gl_translate() { glTranslatef(x, y, z); }

  float modulo() const;

  Point3 normalize() const;

  Point3 operator-() const;

  Point3 &operator+=(const Point3 &other);

  Point3 operator+(const Point3 &other) const;
  Point3 operator-(const Point3 &other) const;
  Point3 operator/(float f) const;
  // redefine % operator to define cross-product
  Point3 operator%(const Point3 &a) const;
};

// Define Vec3 as a snynonim of Point3, out of clarity.
// They share the implementation but differ in behaviors in few rendering
// scenarios.
using Vec3 = Point3;

// a vector that represents (and it is therefore rendered as a) Normal.
struct Normal3 : public Vec3 {
  Normal3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
  Normal3(const Vec3 &vec3);

  void render() const;
};

struct Vertex {
  Point3 point;
  Normal3 normal;

  Vertex(const Point3 &p = Point3());

  void render(bool send_normal = false) const;
};

struct Edge {
public:
  Vertex *v[2]; // due puntatori a Vertice (i due estremi dell'edge)
  // attributi per edge:
};

struct Face {
public:
  Vertex *verts[3]; // tre puntatori a Vertice (i tre vertici del triangolo)

  // costruttore
  Face(Vertex *a, Vertex *b, Vertex *c);

  // attributi per faccia
  Normal3 normal; // normale (per faccia)

  // computa la normale della faccia
  inline void computeNormal() {
    normal = -((verts[1]->point - verts[0]->point) %
               (verts[2]->point - verts[0]->point))
                  .normalize();
  }

  // forse aggiungere un metodo render anche a Face rende il codice più
  // leggibile
};

// A mesh object, loaded from a Wavefront Obj
class Mesh {
private:
  std::vector<Vertex> m_verts; // vettore di vertici
  std::vector<Face> m_faces;   // vettore di facce
  //  std::vector<Edge> m_edges;   // vettore di edge (per ora, non usato)
  // empty constructor. loadMesh must be used instead
  Mesh();

  // Note: the wireframe bool will be retrieved by the "m_env" of whichever
  // class is loading a mesh
  void renderWire();
  void render(bool wireframe = false, bool gouraud_shading = true);

  // use the 2 methods below to setup the mesh
  void init();
  // bounding box: minumum and maximum coordinates
  void computeBoundingBox();
  void computeNormalsPerVertex();

public:
  // friend function to load the mesh instead of exporting the cons
  friend std::unique_ptr<Mesh> loadMesh(const char *mesh_filename);
  Point3 bbmin, bbmax; // bounding box

  // frontend for the render method
  void renderFlat(bool wireframe = false);
  void renderGouraud(bool wireframe = false);

  // center of the axis-aligned bounding box
  // Point3 center();
  Point3 center() { return (bbmin + bbmax) / 2.0; }
};

std::unique_ptr<Mesh> loadMesh(const char *mesh_filename);

using game::Key;   // type for game keys
class SmartWindow; // pre-declared to be used in Env

/* The Env class represents the Environment of the game.
   It handles all the main components of the scene and all the callbacks
   associated with the commands.
*/
class Env final {

private:
  Env(); // constructs the Environment, intializing all the variables.

  double m_fps;     // fps value in the last interval
  double m_fps_now; // fps currently drawn
  uint m_last_time;
  float m_eye_dist, m_view_alpha, m_view_beta; 
  int m_screenH, m_screenW;
  int m_camera_type;
  int m_step; // number of steps of Physics currently done
  bool m_wireframe, m_envmap, m_headlight, m_shadow;

  /* Callbacks variables:
   *  they will be the handler for keys, windows events and rendering.
   *  The actual callback function will vary according to the current
   *  state of the game.
   *  For example, if we are on Menù, the rendering callback will be different
   *  wrt when we are actually playing.
   */
  std::function<void()> m_action_handler, m_render_handler,
      m_window_event_handler;
  std::function<void(game::Key)> m_key_up_handler, m_key_down_handler;

public:
  // Friends can touch your private parts.
  friend Env &get_env();

  // destructor takes care of closing the SDL libraries
  virtual ~Env();

  // accessors
  inline decltype(m_wireframe) isWireframe() { return m_wireframe; }
  inline decltype(m_envmap) isEnvmap() { return m_envmap; }
  inline decltype(m_headlight) isHeadlight() { return m_headlight; }
  inline decltype(m_shadow) isShadow() { return m_shadow; }
  
  inline decltype(m_eye_dist) eyeDist() { return m_eye_dist; }
  inline decltype(m_view_alpha) alpha() {return m_view_alpha; }
  inline decltype(m_view_beta) beta() {return m_view_beta; }

  inline void toggle_wireframe() { m_wireframe = !m_wireframe; }
  inline void toggle_envmap() { m_envmap = !m_envmap; }
  inline void toggle_headlight() { m_headlight = !m_headlight; }
  inline void toggle_shadow() { m_shadow = !m_shadow; }

  // Setters for all the callbacks
  // Default: empty
  void set_action(decltype(m_action_handler) actions = [] {});
  void set_keydown_handler(decltype(m_key_down_handler) onkeydown = [](Key) {});
  void set_keyup_handler(decltype(m_key_up_handler) onkeyup = [](Key) {});
  void set_render(decltype(m_render_handler) render = [] {});
  void set_winevent_handler(decltype(m_window_event_handler) onwinev = [] {});

  std::unique_ptr<SmartWindow> createWindow(std::string &name, size_t x,
                                            size_t y, size_t w, size_t h);
  void clearBuffer();
  void setColor(const Color &color);

  // drawing functions
  void drawFloor(TexID texbind, float sz, float height, size_t num_quads);
  void drawPlane(float sz, float height, size_t num_quads); 
  void drawSky(TexID texbind, double radius, int lats, int longs);
  void drawSphere(double r, int lats, int longs);
  void drawTorus(double r, double R); 
  
  inline void disableLighting() { glDisable(GL_LIGHTING); }
  inline void enableLighting() { glEnable(GL_LIGHTING); }

  void enableDoubleBuffering();
  void enableZbuffer(int depth);
  void enableJoystick();

  Uint32 getTicks();

  void lineWidth(float width);
  // Load texture from an image and return bool if success, should be changed to
  // return a texture ID --i.e. unsigned integer
  TexID loadTexture(const char *filename, bool repeat = false,
                    bool nearest = false);

  // Accepts a lambda to be performed between push and pop
  // Saves time and ensures the matrix will be popped after
  // pushing
  void mat_scope(const std::function<void()> callback);

  /*
   * Important function: main loop, it runs forever till it encounters an
   * SDL_Quit event and dispatch everything.
   */

  void redraw();

  // compute the FPS and Renders!
  void render();
  void renderLoop();

  void rotate(float angle, const Vec3 &axis);
  void scale(float scale_x, float scale_y, float scale_z);

  // set the camera to aim to reference frame (aim_x, aim_y, aim_z) from the
  // observing frame (eye_x,y,z)
  void setCamera(double eye_x, double eye_y, double eye_z, double aim_x,
                 double aim_y, double aim_z, double upX, double upY,
                 double upZ);

  void setCoordToPixel();

  // Model and Projection matrix setup
  void setupModel();
  void setupPersp();

  // Lights setup
  void setupLightPosition();
  void setupModelLights();

  void translate(float scale_x, float scale_y, float scale_z);

  // texture drawing helper function
  void textureDrawing(TexID texbind, std::function<void()> callback,
                      bool gen_coordinates = true);
};

// return singleton instance
Env &get_env();

class SmartWindow {

private:
  SDL_Window *m_win;         // SDL window
  SDL_GLContext m_GLcontext; // SDL OpenGL Context
  std::string m_name;        // window name
  Env &m_env;

public:
  size_t m_width, m_height;

  SmartWindow(std::string &name, size_t x, size_t y, size_t w, size_t h);
  virtual ~SmartWindow();

  void hide();
  void refresh();
  void setupViewport();
  void show();
};
} // namespace agl

#endif // AGL_H_
