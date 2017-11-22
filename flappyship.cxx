#include "ship.h"

namespace elements {
/*                                *
 *                                *
 *    Starring: The FLAPPY ship   *
 *                                *
 */

using namespace spaceship;

FlappyShip::FlappyShip(const char *texture_filename, const char *mesh_filename)
    : Spaceship(texture_filename, mesh_filename) {}

bool FlappyShip::computePhysics() {
  bool steering = updateSteering();
  bool steer_flight = updateSteerFlight();
  bool velocity = updateVelocity();

  return steering || steer_flight || velocity;
}

void FlappyShip::updatePosition() {
  float vel_xm, vel_ym, vel_zm;

  //--- traslation update ---//

  float cosf = cos(m_facing * M_PI / 180.0);
  float sinf = sin(m_facing * M_PI / 180.0);

  // project the speed from the ship reference frame -> world ref frame
  vel_xm = m_speedZ * sinf;
  vel_zm = m_speedZ * cosf;

  // position = position + velocity * delta_t (but the latter is == 1)
  m_px += vel_xm;
  m_py += m_speedY;
  m_pz += vel_zm;

  // limit on Y-motion: we can't get under the floor
  m_py = (m_py < 2.0) ? 2.0 : m_py;

  //--- angular update ---//

  // update the facing angle of the ship according to the computed steering
  // and apply grip
  m_facing -= (m_speedZ * m_grip) * m_steering;
}

bool FlappyShip::updateSteerFlight() {
  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  // if the spaceship is not in the middle of a steering and no key is pressed
  if (!has_velocity() && !throttle && !brake) {
    return false;
  }

  if (throttle ^ brake) {
    int sign = throttle ? 1 : -1;
    m_steer_flight += sign * m_steer_speed;
  }

  // steer return straight back
  m_steer_flight *= m_steer_return;
  return true;
}

// Compute the velocity update during time
// return false if no update is needed
bool FlappyShip::updateVelocity() {
  const static auto FLY_FRICTION = 0.98;
  const static auto GRAVITY = 0.033;

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

    m_speedY -= sign * FLIGHT_ACC;
    m_speedZ += sign * VERY_FAST_ACC;
    // Spaceships don't fly backwards
    m_speedZ = (m_speedZ > 0.05) ? 0 : m_speedZ;
  }

  // apply friction
  m_speedX *= m_frictionX;
  m_speedZ *= m_frictionZ;

  // Air friction and Gravity pulls down the ship!
  m_speedY *= FLY_FRICTION;
  m_speedY -= GRAVITY;
  return true;
}

void FlappyShip::render(bool flicker) {
  m_env.mat_scope([&] {

    // translate the camera to follow the ship movements
    m_env.translate(m_px, m_py, m_pz);

    // rotate the ship according to the facing direction
    m_env.rotate(m_facing, m_viewUP);

    // the Mesh is loaded on the other side
    m_env.rotate(m_rotation_angle, m_viewUP);

    // rotate the ship acc. to steering val, to represent tilting
    auto sign = m_rotation_angle == ENVOS_ANGLE ? -1 : 1;
    m_env.rotate(sign * m_steering, m_front_axis);

    // rotate on the X-axis to represent tilting in flight mode, on the nose of
    // the ship
    agl::Vec3 Xaxis = agl::Vec3(1, 0, 0);
    m_env.rotate(sign * m_steer_flight, Xaxis);

    if (flicker) {
      drawFlicker();
    } else {
      draw();
    }

  });
}

} // namespace elements
