
#include "elements.h"

// Implementation of the objects in elementsh
namespace elements {

/*
* Floor
*/
Floor::Floor(const char *texture_filename)
    : m_size(100.0f), m_height(0.0f), m_env(agl::get_env()),
      // repat = true, linear interpolation
      m_tex(m_env.loadTexture(texture_filename, true)) {}

void Floor::render() {
  //lg::i(__func__, "Rendering floor...");
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
    : m_radius(100.0f), m_lats(20.0f), m_longs(20.0f), m_env(agl::get_env()),
      m_tex(m_env.loadTexture(texture_filename, false)) {}

void Sky::render() {
  //lg::i(__func__, "Rendering Sky...");
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
*  Starring: Spaceship class.
*/

using namespace spaceship;

// get Singleton
std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                         const char *mesh_filename) {
  const static auto TAG = __func__;
  lg::i(TAG, "Loading Spaceship --> texture: %s Mesh: %s", texture_filename,
        mesh_filename);

  /*static std::unique_ptr<Spaceship> s_Spaceship(nullptr);
  if (!s_Spaceship) {
    s_Spaceship.reset(new Spaceship(texture_filename, mesh_filename)); // Init
} */

  return std::unique_ptr<Spaceship>(new Spaceship(texture_filename, mesh_filename));
}

Spaceship::Spaceship(const char *texture_filename,
                     const char *mesh_filename) // da finire
    : m_env(agl::get_env()), m_tex(m_env.loadTexture(texture_filename)),                                 // no texture for now
       m_mesh(agl::loadMesh(mesh_filename)) // TODO
      {
          init();
      }

void Spaceship::init() {
  m_scaleX = m_scaleY = m_scaleZ = 0.6;

  m_px = m_pz = 0;
  m_py = 1.0; // Spaceship skills™

  m_facing = m_steering = m_angle = 0.0;
  m_speedX = m_speedY = m_speedZ = 0.0;

  // strong friction on X-axis, if you wanna drift, go buy Need For Speed
  m_frictionX = 0.9;
  // no fritcion on Y-axis, we're not gonna fly vertical. For now.
  m_frictionY = 1.0;
  // small friction on Z-axis
  m_frictionZ = 0.991;

  m_steer_speed = 3.4;         // A
  m_steer_return_speed = 0.93; // B ==> max steering = A*B / (1-B) == 2.4

  m_max_acceleration = 0.0011;

  //init internal states
  m_state = {false};
}

bool Spaceship::get_state(Motion mt) { return m_state[mt]; }

void Spaceship::execute() {
    //later on will be added an abstraction on doMotion
    //and doMotion will be divided into several modular method
    processCommand();
    doMotion();
}

void Spaceship::processCommand() {
  const static auto TAG = __func__;

  if (!m_cmds.empty()) {
    // read and pop command
    Command cmd = m_cmds.front();
    m_cmds.pop();

    // get command name in string in order to log
    std::string mt = motion_to_str(cmd.first);
    lg::i(TAG, "Spaceship is processing command %s", mt.c_str());

    // set the state
    m_state[cmd.first] = cmd.second;
  }
}

void Spaceship::sendCommand(Motion motion, bool on_off) {
  if (motion >= Motion::N_MOTION) {
    lg::panic(__func__, "Command not recognized!!");
  }

  // construct and submit a new pair
  m_cmds.emplace(motion, on_off);
}

void Spaceship::setScale(float x, float y, float z) {
  m_scaleX = x;
  m_scaleY = y;
  m_scaleZ = z;
}

void Spaceship::shadow() const {
  const auto c = agl::SHADOW;

  m_env.setColor(c);
  m_env.translate(0, 0.01, 0); // avoid z-fighting with the floor(
  m_env.scale(1.01, 0, 1.01);  // squash on Y, 1% scaling-up on X and Z

  m_env.disableLighting();
  // renderAllParts
  m_env.enableLighting();
}

// draw the ship as a textured mesh, using the helper functions defined
// in the Env class.
void Spaceship::draw() const {
   m_env.textureDrawing(m_tex, [&]{
          m_env.mat_scope([&] {
            m_env.scale(m_scaleX, m_scaleY, m_scaleZ);

            m_mesh->renderGouraud(m_env.isWireframe());
           });
  }); // generate coords automatically, true by default

  // if headlight is on in the Env, then draw headlights
  if (m_env.isHeadlight()) {
    //drawHeadlight(0, 0, -1, 10);
  }
}

// attiva una luce di openGL per simulare un faro
void Spaceship::drawHeadlight(float x, float y, float z, int lightN) const {

  int usedLight = GL_LIGHT1 + lightN;

  glEnable(usedLight);

  float col0[4] = {0.8, 0.8, 0.0, 1};
  glLightfv(usedLight, GL_DIFFUSE, col0);

  float col1[4] = {0.5, 0.5, 0.0, 1};
  glLightfv(usedLight, GL_AMBIENT, col1);

  float tmpPos[4] = {x, y, z, 1}; // ultima comp=1 => luce posizionale
  glLightfv(usedLight, GL_POSITION, tmpPos);

  float tmpDir[4] = {0, 0, -1, 0}; // ultima comp=1 => luce posizionale
  glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir);

  glLightf(usedLight, GL_SPOT_CUTOFF, 30);
  glLightf(usedLight, GL_SPOT_EXPONENT, 5);

  glLightf(usedLight, GL_CONSTANT_ATTENUATION, 0);
  glLightf(usedLight, GL_LINEAR_ATTENUATION, 1);
}

void Spaceship::render() const {
  m_env.mat_scope([&] {
    agl::Vec3 viewUP = agl::Vec3(0, 1, 0);
    agl::Vec3 front = agl::Vec3(0, 0, 1);
    lg::i(__func__, "translating to position %f %f %f", m_px, m_py, m_pz);
                 // translate the ship according to its coordinates
                 m_env.translate(m_px, m_py, m_pz);
    // rotate the ship according to its angle and facing
    m_env.rotate(m_angle, viewUP);
    // steering

    m_env.rotate(-m_steering, front);

    draw();
});
}

void Spaceship::rotateView() {
  agl::Vec3 axisX = agl::Vec3(1, 0, 0);
  agl::Vec3 axisY = agl::Vec3(0, 1, 0);

  m_env.rotate(m_view_beta, axisX);
  m_env.rotate(m_view_alpha, axisY);
}

// DA FARE + MODULARE
void Spaceship::doMotion() {

  // computiamo l'evolversi della macchina
  float speed_xm, speed_ym, speed_zm; // velocita' in spazio macchina

  // da vel frame mondo a vel frame macchina
  float cosf = cos(m_facing * M_PI / 180.0);
  float sinf = sin(m_facing * M_PI / 180.0);
  speed_xm = +cosf * m_speedX - sinf * m_speedZ;
  speed_ym = m_speedY;
  speed_zm = +sinf * m_speedX + cosf * m_speedZ;

  bool left = get_state(Motion::STEER_L);
  bool right = get_state(Motion::STEER_R);

  if (left ^ right) {
    int sign = left ? 1 : -1;

    m_steering += sign * m_steer_speed;
    m_steering *= m_steer_return_speed;
  }

  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  if (throttle ^ brake) {
     lg::i(__func__, "lolo");
    int sign = throttle ? 1 : -1;

    m_speedZ -= sign * m_max_acceleration;

    // Spaceships don't fly backwards
    m_speedZ = (m_speedZ > 0.05) ? 0 : m_speedZ;
  }

  speed_xm *= m_frictionX;
  speed_ym *= m_frictionY;
  speed_zm *= m_frictionZ;

  // l'orientamento della macchina segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  m_facing = m_facing - (speed_zm * m_grip) * m_steering;

  /* // rotazione mozzo ruote (a seconda della velocita' sulla z)
   float da; // delta angolo
   da = (360.0 * speed_zm) / (2.0 * M_PI * raggioRuotaA);
   mozzoA += da;
   da = (360.0 * speed_zm) / (2.0 * M_PI * raggioRuotaP);
   mozzoP += da; */

  // ritorno a vel coord mondo
  m_speedX = +cosf * speed_xm + sinf * speed_zm;
  m_speedY = speed_ym;
  m_speedZ = -sinf * speed_xm + cosf * speed_zm;

  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  m_px += m_speedX;
  m_py += m_speedY;
  m_pz += m_speedZ;
}

const std::string motion_to_str(Motion m) {
  switch (m) {
  case Motion::THROTTLE:
    return "THROTTLE";

  case Motion::STEER_R:
    return "RIGHT STEERING";

  case Motion::STEER_L:
    return "LEFT STEERING";

  case Motion::BRAKE:
    return "BRAKE";

  default:
    // shouldn't arrive here
    return "Motion not recognized!!";
    lg::panic(__func__, "!! Motion not recognized !!");
  }
 }
}
