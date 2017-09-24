#include "elements.h"

namespace elements {
    /*                                *
 *                                *
 *  Starring: Spaceship class.    *
 *                                *
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

  return std::unique_ptr<Spaceship>(new Spaceship(texture_filename,
                                                  mesh_filename)); // init
}

Spaceship::Spaceship(const char *texture_filename,
                     const char *mesh_filename) // da finire
    : m_env(agl::get_env()),
      m_tex(m_env.loadTexture(texture_filename)), // no texture for now
      m_mesh(agl::loadMesh(mesh_filename))        // TODO
{
  init();
}

void Spaceship::init() {
  // The "Envos" spaceship mesh is huge. Here we set proper re-scaling.
  m_scaleX = m_scaleY = m_scaleZ = ENVOS_SCALE;

  m_px = m_pz = 0;
  m_py = 2.0; // Spaceship skills™

  m_facing = m_steering = 0.0;
  m_speedX = m_speedY = m_speedZ = 0.0;

  //-- CONSTANTS --//
  //---------------//

  m_viewUP = agl::Vec3(0, 1, 0);
  m_front_axis = agl::Vec3(0, 0, 1);

  // strong friction on X-axis, if you wanna drift, go buy Need For Speed
  m_frictionX = 0.9;
  // no fritcion on Y-axis, we're not gonna fly vertical. For now.
  m_frictionY = 1.0;
  // small friction on Z-axis
  m_frictionZ = 0.991;

  m_steer_speed = 3.4;   // A
  m_steer_return = 0.93; // B ==> max steering = A*B / (1-B) == 2.4

  m_max_acceleration = 0.0058;
  m_grip = 0.45;

  // init internal states
  m_state = {false};
}

// draw the ship as a textured mesh, using the helper functions defined
// in the Env class.
void Spaceship::draw() const {
  m_env.textureDrawing(m_tex, [&] {
    m_env.mat_scope([&] {
      m_env.scale(m_scaleX, m_scaleY, m_scaleZ);

      m_mesh->renderGouraud(m_env.isWireframe());
    });
  }); // generate coords automatically, true by default

  // if headlight is on in the Env, then draw headlights
  if (m_env.isHeadlight()) {
    // drawHeadlight(0, 0, -1, 10);
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

// DA FARE + MODULARE
void Spaceship::doMotion() {
  // Here we compute the evolution of the Spaceship during time

  float vel_xm, vel_ym, vel_zm; // velocita in spazio macchina

  // da vel frame mondo a vel frame macchina
  float cosf = cos(m_facing * M_PI / 180.0);
  float sinf = sin(m_facing * M_PI / 180.0);
  vel_xm = +cosf * m_speedX - sinf * m_speedZ;
  // vel_ym = m_speedY;
  vel_zm = +sinf * m_speedX + cosf * m_speedZ;

  // *** Velocity Update *** //
  // ----------------------- //

  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  if (throttle ^ brake) {
    int sign = throttle ? -1 : 1;

    vel_zm += sign*m_max_acceleration;
    // Spaceships don't fly backwards
    vel_zm = (vel_zm > 0.05) ? 0 : vel_zm;
  }

  vel_xm *= m_frictionX;
  // vel_ym *= m_frictionY;
  vel_zm *= m_frictionZ;

  // *** Steering Update *** //
  // ----------------------- //

  bool left = get_state(Motion::STEER_L);
  bool right = get_state(Motion::STEER_R);

 if (left ^ right) {
    int sign = left ? 1 : -1;
    m_steering += sign * m_steer_speed;
  }
  // steer return straight back
  m_steering *= m_steer_return;

  m_facing = m_facing - (vel_zm * m_grip) * m_steering;

  // ritorno a vel coord mondo
  m_speedX = +cosf * vel_xm + sinf * vel_zm;
  //m_speedY = vel_ym;
  m_speedZ = -sinf * vel_xm + cosf * vel_zm;

  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  m_px += m_speedX;
  // m_py += m_speedY;
  m_pz += m_speedZ;
}

// ONLY TO BE USED IN FLIGHT MODE:
// it updates y-values to make the spaceship fly vertically
void Spaceship::updateFly() {
  const static auto FLY_FRICTION = 0.98;
  const static auto FLY_RETURN = 0.060;

  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  if (throttle ^ brake) {
    int sign = throttle ? 1 : -1;

    m_speedY += sign*0.1;
  }

  m_speedY*= FLY_FRICTION;
  m_speedY-= FLY_RETURN;

  m_py += m_speedY;
  // limit on Y-motion: we can't get under the floor
  m_py = (m_py < 1) ? 1 : m_py;
}

void Spaceship::execute() {
  // later on will be added an abstraction on doMotion
  // and doMotion will be divided into several modular method
  processCommand();
  doMotion();
  //updateFly();
}

bool Spaceship::get_state(Motion mt) { return m_state[mt]; }

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

void Spaceship::processCommand() {
  const static auto TAG = __func__;

  if (!m_cmds.empty()) {
    // read and pop command
    Command cmd = m_cmds.front();
    m_cmds.pop();

    // get command name in string in order to log
    std::string mt = motion_to_str(cmd.first);
   // lg::i(TAG, "Spaceship is processing command %s", mt.c_str());

    // set the state
    m_state[cmd.first] = cmd.second;
  }
}

void Spaceship::render() const {
  m_env.mat_scope([&] {

    agl::Vec3 front_boat = agl::Vec3(1, 0, 0);

    // translate the camera to follow the ship movements
    m_env.translate(m_px, m_py, m_pz);

    // rotate the ship according to the facing direction
    m_env.rotate(m_facing, m_viewUP);

    // the Mesh is loaded on the other side
    m_env.rotate(180, m_viewUP);

    // rotate the ship acc. to steering val, to represent tilting
    int sign = -1;
    m_env.rotate(sign * m_steering, m_front_axis);

    draw();
  });
}

void Spaceship::rotateView(int32_t view_alpha, int32_t view_beta) {
  agl::Vec3 axisX = agl::Vec3(1, 0, 0);
  agl::Vec3 axisY = agl::Vec3(0, 1, 0);

  // avoid ending up under the ship
  view_beta = (view_beta < 5) ? 5 : view_beta;
  view_beta = (view_beta > 90) ? 90 : view_beta;

  m_env.rotate(view_beta, axisX);
  m_env.rotate(view_alpha, axisY);
}

void Spaceship::sendCommand(Motion motion, bool on_off) {
  if (motion >= Motion::N_MOTION) {
    lg::panic(__func__, "Command not recognized!!");
  }

  // construct and submit a new pair
  m_cmds.emplace(motion, on_off);
}

void Spaceship::scale(float x, float y, float z) {
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

}
