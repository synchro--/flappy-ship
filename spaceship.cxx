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

void Spaceship::init(bool truman) {
  // The "Envos" spaceship mesh is huge. Here we set proper re-scaling.
  m_scaleX = m_scaleY = m_scaleZ = truman ? BOAT_SCALE : ENVOS_SCALE; 
  m_rotation_angle = truman ? BOAT_ANGLE : ENVOS_ANGLE;

  m_px = 0.0;
  m_pz = 0.0;
  m_py = 2.0; // Spaceship skills™

  m_facing = m_steering = 0.0;
  m_speedX = m_speedY = m_speedZ = 0.0;

  //-- CONSTANTS --//
  //---------------//

  m_viewUP = agl::Vec3(0, 1, 0);
  m_front_axis = truman ? agl::Vec3(1, 0, 0) : agl::Vec3(0,0,1);

  // strong friction on X-axis, if you wanna drift, go buy Need For Speed
  m_frictionX = 0.9;
  // no fritcion on Y-axis, we're not gonna fly vertical. For now.
  m_frictionY = 1.0;
  // small friction on Z-axis
  m_frictionZ = 0.991;

  m_steer_speed = 3.1;   // A
  m_steer_return = 0.93; // B ==> max steering = A*B / (1-B) == 2.4

  m_max_acceleration = FAST_ACC;
  m_grip = 0.45;

  // init internal states
  m_state = {false};

  // init queue: common idiom for clearing standard containers 
  // is swapping with an empty version of the container:
  decltype(m_cmds)().swap(m_cmds);
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
    // lg::i(__func__, "Headlights toggled!");
    drawHeadlight(0, 0, -1, 8);
  }
}

void Spaceship::drawFlicker() const {
  m_env.textureDrawing(m_tex, [&] {
    m_env.mat_scope([&] {
      m_env.scale(m_scaleX, m_scaleY, m_scaleZ);

      m_mesh->renderGouraud(true);
    });
  }); // generate coords automatically, true by default

  // if headlight is on in the Env, then draw headlights
  if (m_env.isHeadlight()) {
    // lg::i(__func__, "Headlights toggled!");
    drawHeadlight(0, 0, -1, 8);
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

void Spaceship::doMotion() {
  // Here we compute the evolution of the Spaceship during time

  bool done_something = computePhysics();

  // if something happened, we need to update the position of the ship
  if (done_something) {
    updatePosition();
  }
}

bool Spaceship::computePhysics() {
  bool steering = updateSteering();
  bool velocity = updateVelocity();

  return steering || velocity;
}

// process the next command in the queue and execute the motion
void Spaceship::execute() {
  processCommand();
  doMotion();
  // still has to develop a 2nd kind of ship
  // updateFly();
}

void Spaceship::updatePosition() {
  float vel_xm, vel_ym, vel_zm;

  //--- traslation update ---//

  float cosf = cos(m_facing * M_PI / 180.0);
  float sinf = sin(m_facing * M_PI / 180.0);

  // project the speed from the ship reference frame -> world ref frame
  vel_xm = m_speedZ * sinf;
  vel_zm = m_speedZ * cosf;

  // position = position + velocity * delta_t (but the latter is == 1)
  m_px += vel_xm;
  // m_py += m_speedY;
  m_pz += vel_zm;

  //--- angular update ---//

  // update the facing angle of the ship according to the computed steering
  // and apply grip
  m_facing -= (m_speedZ * m_grip) * m_steering;
}

// Compute the velocity update during time
// return false if no update is needed
bool Spaceship::updateVelocity() {

  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  // if the spaceship is still and no key is pressed, then return
  if (!has_velocity() && !brake && !throttle) {
    return false;
  }

  // if both THROTTLE and BRAKE (aka W-S) have been pressed,
  // they cancel out each other
  // so we compute the update only if the XOR returns true

  if (throttle ^ brake) {
    int sign = throttle ? -1 : 1;

    m_speedZ += sign * m_max_acceleration;
    // Spaceships don't fly backwards
    m_speedZ = (m_speedZ > 0.05) ? 0 : m_speedZ;
  }

  // apply friction
  m_speedX *= m_frictionX;
  m_speedZ *= m_frictionZ;
  return true;
}

// Compute the steering update
// return false if no update is needed
bool Spaceship::updateSteering() {

  bool left = get_state(Motion::STEER_L);
  bool right = get_state(Motion::STEER_R);

  // if the spaceship is not in the middle of a steering and no key is pressed
  if (!is_steering() && !left && !right) {
    return false;
  }

  if (left ^ right) {
    int sign = left ? 1 : -1;
    m_steering += sign * m_steer_speed;
  }

  // steer return straight back
  m_steering *= m_steer_return;
  return true;
}

// ONLY TO BE USED IN FLIGHT MODE:
// it updates y-values to make the spaceship fly vertically
// NEED TO BE CHANGED LIKE WITH STEERING
// -flying curvature variable etc
void Spaceship::updateFly() {
  const static auto FLY_FRICTION = 0.98;
  const static auto FLY_RETURN = 0.060;

  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  if (throttle ^ brake) {
    int sign = throttle ? 1 : -1;

    m_speedY += sign * 0.1;
  }

  m_speedY *= FLY_FRICTION;
  m_speedY -= FLY_RETURN;

  m_py += m_speedY;
  // limit on Y-motion: we can't get under the floor
  m_py = (m_py < 1) ? 1 : m_py;
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
    // std::string mt = motion_to_str(cmd.first);
    // lg::i(TAG, "Spaceship is processing command %s", mt.c_str());

    // set the state
    m_state[cmd.first] = cmd.second;
  }
}

void Spaceship::render(bool flicker) {
  m_env.mat_scope([&] {

    // translate the camera to follow the ship movements
    m_env.translate(m_px, m_py, m_pz);

    // rotate the ship according to the facing direction
    m_env.rotate(m_facing, m_viewUP);

    // the Mesh is loaded on the other side
    m_env.rotate(m_rotation_angle, m_viewUP);

    // rotate the ship acc. to steering val, to represent tilting
    int sign = m_rotation_angle == ENVOS_ANGLE ? -1 : 1; 
    m_env.rotate(sign * m_steering, m_front_axis);
    //   m_env.rotate(sign * m_steering, front_boat);

    if (flicker) {
      drawFlicker();
    } else {
      draw();
    }

  });
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

void Spaceship::shadow() {
  m_env.mat_scope([&] {
    const auto c = agl::SHADOW;

    m_env.setColor(c);
    m_env.disableLighting();

    m_env.translate(m_px + 2.0, 0.01,
                    m_pz + 2.0); // avoid z-fighting with the floor
    // rotate the ship according to the facing direction
    m_env.rotate(m_facing, m_viewUP);
    // the Mesh is loaded on the other side
    m_env.rotate(ENVOS_ANGLE, m_viewUP);
    // rotate the ship acc. to steering val, to represent tilting
    int sign = -1;
    m_env.rotate(sign * m_steering, m_front_axis);

    m_env.scale(ENVOS_SCALE * 1.01, ENVOS_SCALE * 0.0,
                ENVOS_SCALE * 1.01); // squash on Y, 1% scaling-up on X and Z

    // render the ship without lighting and squashed!
    m_mesh->renderGouraud(m_env.isWireframe());
    m_env.enableLighting();
  });
}

} // namespace elements
