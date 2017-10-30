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
#include <SDL2/SDL_ttf.h>

#include "log.h"
#include "types.h"

/*
 * AGL: Abstract Graphic Library
 * The purpose is to create an abstract layer on the top of OpenGL in order to
 * simplify the usage of all the graphic functions in the project.
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

using game::Key;        // custom type for game keys
using game::MouseEvent; // custom type for mouse events

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
  int m_screenH, m_screenW;
  int m_camera_type;
  int m_step; // number of steps of Physics currently done
  bool m_wireframe, m_envmap, m_headlight, m_shadow, m_blending;

  /* Callbacks variables:
   *  they will be the handler for keys, mouse & windows events and rendering.
   *  The actual callback function will vary according to the current
   *  state of the game.
   *  For example, if we are on Menu, the rendering callback will be different
   *  wrt when we are actually playing.
   */
  std::function<void()> m_action_handler, m_render_handler,
      m_window_event_handler;
  std::function<void(game::Key)> m_key_up_handler, m_key_down_handler;
  std::function<void(game::MouseEvent, int32_t, int32_t)> m_mouse_event_handler;

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
  inline decltype(m_blending) isBlending() { return m_blending; }
  inline decltype(m_screenH) get_win_height() { return m_screenH; }
  inline decltype(m_screenW) get_win_width() { return m_screenW; }
  inline decltype(m_fps) get_fps() { return m_fps; }

  /*
    inline decltype(m_eye_dist) eyeDist() { return m_eye_dist; }
    inline decltype(m_view_alpha) alpha() {return m_view_alpha; }
    inline decltype(m_view_beta) beta() {return m_view_beta; } */

  inline void toggle_wireframe() { m_wireframe = !m_wireframe; }
  inline void toggle_envmap() { m_envmap = !m_envmap; }
  inline void toggle_headlight() { m_headlight = !m_headlight; }
  inline void toggle_shadow() { m_shadow = !m_shadow; }
  inline void toggle_blending() { m_blending = !m_blending; }

  // Setters for all the callbacks
  // Default: empty
  void set_action(decltype(m_action_handler) actions = [] {});
  void set_keydown_handler(decltype(m_key_down_handler) onkeydown = [](Key) {});
  void set_keyup_handler(decltype(m_key_up_handler) onkeyup = [](Key) {});
  void set_mouse_handler(decltype(m_mouse_event_handler) onmousev =
                             [](MouseEvent, int32_t, int32_t) {});
  void set_render(decltype(m_render_handler) render = [] {});
  void set_winevent_handler(decltype(m_window_event_handler) onwinev = [] {});

  std::unique_ptr<SmartWindow> createWindow(std::string &name, size_t x,
                                            size_t y, size_t w, size_t h);
  void clearBuffer();
  void setColor(const Color &color);

  // drawing functions
  void drawCubeFill(const float side);
  void drawCubeWire(const float side);
  void drawCube(const float side);
  void drawFloor(TexID texbind, float sz, float height, size_t num_quads);
  void drawPlane(float sz, float height, size_t num_quads);
  void drawSky(TexID texbind, double radius, int lats, int longs);
  void drawSphere(double r, int lats, int longs);
  void drawSquare(const float side);
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

/*
 * SmartWindow is a class that represents a graphical window, it's basically
 * a wrapper on top of an SDL_Window.
 */

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
  void printOnScreen(std::function<void()> fn);
  void colorWindow(const Color &color);
  void textureWindow(TexID texbind);
};

/* __FONTS__
 * A light and fast library to load and use TTF in OpenGL.
 * The only way to do render TTF in OpenGL is to render each glyph as a texture,
 * which of course carries a painful overhead on the game at runtime. The
 * solution is to load an atlas of chars of the selected TTF as a vector of
 * textures and store in memory. Later on, when we'll need to render text we'll
 * just render it as a list of textured quads from pre-loaded textures.
 */

// starting offset of the ASCII chars
static const auto ASCII_SPACE_CODE = 0x20;
// ending offset of the ASCII chars
static const auto ASCII_DEL_CODE = 0x7F;

// optimized X GLubyte version of a Glyph
class Glyph {
private:
  // members
  char m_letter; // freetype glyph index
  TexID m_texID;

  GLubyte m_minx;
  GLubyte m_miny;
  GLubyte m_maxx;
  GLubyte m_maxy;
  GLubyte m_advance; // number of pixels to advance on x axis

public:
  // accessors
  // Note: accessors use decltype as the exact type of each member
  // can be changed due to memory optimization

  inline decltype(m_letter) get_letter() { return m_letter; }
  inline decltype(m_texID) get_textureID() { return m_texID; }
  inline decltype(m_advance) get_advance() { return m_advance; }
  inline decltype(m_minx) get_minX() { return m_minx; }
  inline decltype(m_miny) get_minY() { return m_miny; }
  inline decltype(m_maxx) get_maxX() { return m_maxx; }
  inline decltype(m_maxy) get_maxY() { return m_maxy; }

  Glyph(char letter, TexID textureID, GLubyte minx, GLubyte maxx, GLubyte miny,
        GLubyte maxy, GLubyte advance);
};

// Abstract GL TextRenderer
// Responsible of loading the TTF and initialize the texture atlas vec.

class AGLTextRenderer {
private:
  // the texture atlas vector
  std::vector<Glyph> m_glyphs;
  int m_font_outline;
  int m_font_height;
  TTF_Font *m_font_ptr;
  Env &m_env; // cache envinronment

  inline Glyph &get_glyph_at(size_t index) { return m_glyphs.at(index - ' '); }

  void loadTextureVector();

  // prevent to call cons, use friend function instead
  AGLTextRenderer(const char *font_path, size_t font_size);
  void renderChar(int x_o, int y_o, char letter);

public:
  int render(int x_o, int y_o, const char *str);
  // same as above but for std::string
  int render(int x_o, int y_o, std::string &str);
  int renderf(int x_o, int y_o, const char *fmt, ...);
  int get_width(const char *str);

  inline decltype(m_font_height) get_height() { return m_font_height; }

  // quit gracefully
  virtual ~AGLTextRenderer();
  // return singleton instance
  // friend AGLTextRenderer *getTextRenderer(const char *font_path,
  //                                         size_t font_size);
  friend std::unique_ptr<AGLTextRenderer> getTextRenderer(const char *font_path,
                                                          size_t font_size);
};

// AGLTextRenderer *getTextRenderer(const char *font_path, size_t font_size);
std::unique_ptr<AGLTextRenderer> getTextRenderer(const char *font_path,
                                                 size_t font_size);
} // namespace agl

#endif // AGL_H_
