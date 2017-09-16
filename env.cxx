#include "agl.h"
#include <SDL2/SDL_ttf.h>

namespace agl {
// Returns the singleton instance of agl::Env, initializing it if necessary
Env &get_env() {
  // having a unique ptr ensures the Env will be called only during the main
  // static std::unique_ptr<Env> s_env(nullptr);
  static std::unique_ptr<Env> s_env(nullptr);
  if (!s_env) {
    s_env.reset(new Env()); // initialize the environment
  }

  return *s_env;
}

// constructs the environment, initializing many elements
Env::Env()
    // All callbacks are init to empty lambdas
    : m_action_handler([] {}), m_render_handler([] {}),
      m_window_event_handler([] {}), m_key_down_handler([](Key) {}),
      m_key_up_handler([](Key) {}),

      // all environment variables
      m_eye_dist(5.0), m_screenH(750), m_screenW(750), m_wireframe(false),
      m_envmap(true), m_headlight(false), m_shadow(true), m_step(0) {

  // "__func__" == function name
  static const auto TAG = __func__;

  // Don't let SDL set its signal() handlers - Ctrl+c and friends will work
  SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

  lg::i(TAG, "init SDL and OpenGL");

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
    lg::e(TAG, "Env::Env", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (TTF_Init() < 0) {
    lg::e(TAG, "Env::Env", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  enableZbuffer(16);
  enableDoubleBuffering();

  logs::i(TAG, "SDL and OpenGL Init: done");
}

// quit gracefully
Env::~Env() {
  lg::i(TAG, "Quit...");

  TTF_Quit();
  SDL_Quit();
}

// Sets the action callback which is called once in every iteration. Before the
// actual rendering.
void Env::set_action(decltype(m_action_handler) actions) {
  m_action_handler = actions;
}

// Sets the onkeydown callback.
void Env::set_keydown_handler(decltype(m_key_down_handler) onkeydown) {
  m_key_down_handler = onkeydown;
}

// Sets the onkeyup callback.
void Env::set_keyup_handler(decltype(m_key_up_handler) onkeyup) {
  m_key_up_handler = onkeyup;
}

// Sets the onwindowevent callback, which is called when the window is exposed
// again after being covered - usually it is set to the same as
// m_render_handler.
void Env::set_winevent_handler(decltype(m_window_event_handler) onwinev) {
  m_window_event_handler = onwinev;
}

// Sets the m_render_handler callback, which is called to render the scene.
void Env::set_render(decltype(m_render_handler) render) {
  m_render_handler = render;
}

void Env::enableZbuffer(int depth) {
  lg::i(__func__, "Enabling Z-Buffer of depth %d", depth);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
}

void Env::enableDoubleBuffering() {
  lg::i(__func__, "Enabling double-buffer")
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

// enables joystick
void Env::enableJoystick() {
  lg::i(__func__, "### Joystick is Enabled ###");

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);
}

void Env::mat_scope(const std::function<void(void)> callback) {
  glPushMatrix();
  callback();
  glPopMatrix();
}

Uint32 Env::getTicks() { return SDL_GetTicks(); }

void setColor(const Color &color) {
  glColor4f(color.r, color.g, color.b, color.a);
}

void Env::clearBuffer() {
  Color c = Color::WHITE;
  // colore sfondo = bianco
  glClearColor(c.r, c.g, c.b, c.a);
  // riempe tutto lo screen buffer di pixel color sfondo
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Env::drawFloor(TexID texbind, float sz, float height, size_t num_quads) {

  textureDrawing(texbind,
                 [&] {
                   // draw num_quads^2 number of quads
                   glBegin(GL_QUADS);
                   glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
                   glNormal3f(0, 1, 0);      // normale verticale uguale x tutti
                   for (int x = 0; x < num_quads; x++)
                     for (int z = 0; z < num_quads; z++) {
                       float x0 = -sz + 2 * (x + 0) * sz / num_quads;
                       float x1 = -sz + 2 * (x + 1) * sz / num_quads;
                       float z0 = -sz + 2 * (z + 0) * sz / num_quads;
                       float z1 = -sz + 2 * (z + 1) * sz / num_quads;
                       glVertex3d(x0, height, z0);
                       glVertex3d(x1, height, z0);
                       glVertex3d(x1, height, z1);
                       glVertex3d(x0, height, z1);
                     }
                   glEnd();

                 },
                 false)
}

// hint: should be 100.0 20.0 20.0 --> see Sky constructor
void Env::drawSky(TexID texbind, double radius, int lats, int longs) {

  textureDrawing(texbind,
                 [&] {

                   if (m_wireframe) {
                     glDisable(GL_TEXTURE_2D);
                     glColor3f(BLACK.r, BLACK.g, BLACK.b);
                     glDisable(GL_LIGHTING);
                     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                     drawSphere(radius, lats, longs);

                     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                     glColor3f(WHITE.r, WHITE.g, WHITE.b);
                     glEnable(GL_LIGHTING);
                   } else {
                     glBindTexture(GL_TEXTURE_2D, texbind);
                     glEnable(GL_TEXTURE_2D);
                     glEnable(GL_TEXTURE_GEN_S);
                     glEnable(GL_TEXTURE_GEN_T);
                     glTexGeni(GL_S, GL_TEXTURE_GEN_MODE,
                               GL_SPHERE_MAP); // Env map
                     glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                     glColor3f(WHITE.r, WHITE.g, WHITE.b);
                     glDisable(GL_LIGHTING);

                     drawSphere(radius, lats, longs);

                     glDisable(GL_TEXTURE_GEN_S);
                     glDisable(GL_TEXTURE_GEN_T);
                     glDisable(GL_TEXTURE_2D);
                     glEnable(GL_LIGHTING);
                   }

                 },
                 true)
}

void Env::drawSphere(double radius, int lats, int longs) {
  int i, j;
  for (i = 0; i <= lats; i++) {
    double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
    double z0 = sin(lat0);
    double zr0 = cos(lat0);

    double lat1 = M_PI * (-0.5 + (double)i / lats);
    double z1 = sin(lat1);
    double zr1 = cos(lat1);

    glBegin(GL_QUAD_STRIP);
    for (j = 0; j <= longs; j++) {
      double lng = 2 * M_PI * (double)(j - 1) / longs;
      double x = cos(lng);
      double y = sin(lng);

      // Normal are needed for the EnvMap
      glNormal3f(x * zr0, y * zr0, z0);
      glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
      glNormal3f(x * zr1, y * zr1, z1);
      glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
    }
    glEnd();
  }
}

// Load texture from image file.
// repeat == true --> GL_REPEAT for s and t coordinates
// nearest == true --> apply neareast neighbour interpolation
/*
* N.B: While linear interpolation gives a smoother result,
* it isn't always the most ideal option. Nearest neighbour interpolation
* is more suited in games that want to mimic 8 bit graphics,
* because of the pixelated look.
*/

Env::TexID Env::loadTexture(const char *filename, bool repeat, bool nearest) {

  lg::i(__func__, "Loading texture from file %s", filename);

  SDL_Surface *s = IMG_Load(filename);
  if (!s) {
    lg::e(__func__, "Error while loading texture from file %s", filename);
    return EXIT_FAILURE;
  }

  TexID texbind;
  // generate a name for the texture (i.e. an unsigned int)
  glGenTextures(1, &texbind);
  glBindTexture(GL_TEXTURE_2D, texbind);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, s->w, s->h, GL_RGB, GL_UNSIGNED_BYTE,
                    s->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  nearest ? GL_NEAREST : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);

  if (repeat) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  return texbind;
}

// probabilmente da eliminare e gestire diversamente in seguito
void Env::redraw() {
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type = SDL_WINDOWEVENT;
  e.window.event = SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}

void Env::rotate(float angle, const Vec3 &axis) {
  glRotatef(angle, axis.x, axis.y, axis.z);
}

void Env::scale(float x, float y, float z) { glScalef(x, y, z); }

void Env::set_camera(double eye_x, double eye_y, double eye_z, double aim_x,
                     double aim_y, double aim_z) {
  // up vector looking at the sky (0,+y,0)
  gluLookAt(eye_x, eye_y, eye_z, aim_x, aim_y, aim_z, 0, 1, 0);
}

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schemo
void Env::setCoordToPixel() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1, -1, 0);
  glScalef(2.0 / m_screenW, 2.0 / m_screenH, 1);
}

void Env::setupLightPosition() {
  // setto la posizione luce
  float light_position[4] = {0, 1, 2, 0}; // ultima comp=0 => luce direzionale
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void Env::setupModelLights() {
  // setup lights for the model
  static float params[4] = {1, 1, 1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, params);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  glEnable(GL_LIGHTING);
}

// Switches mode into GL_MODELVIEW, and then loads an identity matrix.
void Env::setupModel() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Switches mode into GL_PERSPECTIVE, and then loads an identity matrix.
// The perspective is then arbitrarily set up.
// Height and Width are hardcoded for now
void Env::setupPersp() {
  double fovy = 70.0, // field of view angle, in deegres, along y direction
      zNear = .2, zFar = 1000; // clipping plane distance

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, m_screenW / m_screenH, zNear, zFar);
}

// Helper function to draw textured objects
// Accepts a lambda as a drawing function to be called afte the texture
// is applied.
void Env::textureDrawing(TexID texbind, std::function<void()> callback,
                         bool gen_coordinates) {

  glBindTexture(GL_TEXTURE_2D, texbind);
  glEnable(GL_TEXTURE_2D);

  // if the surface is complex, let OpenGL generate the coords for you
  if (gen_coordinates) {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
  }

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE,
            m_envmap ? GL_SPHERE_MAP : GL_OBJECT_LINEAR); // EnvMap
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE,
            envmap ? GL_SPHERE_MAP : GL_OBJECT_LINEAR);

  //--- call the callback drawing function --- //
  callback();

  if (gen_coordinates) {
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_S);
  }

  // disable texturing
  glDisable(GL_TEXTURE_2D);
}

void Env::translate(float x, float y, float z) { glTranslatef(x, y, z); }
}
