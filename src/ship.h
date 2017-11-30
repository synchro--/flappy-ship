#ifndef _SHIP_H
#define _SHIP_H

#include "types.h"

#include "agl.h"
#include "log.h"

/*
 * The Spaceship class.
 *
 * STATE and MOTION:
 * The motion of the spaceship is defined by the 4 state variables:
 * THROTTLE - STEER_L - STEER_R - BRAKE that are either 'off' or 'on'.
 * When the player send a key, the action will be stored in a queue in the form
 * of <State Variable , on/off > (here called Motion).
 * Thus, the Spaceship execute commands by simply reading from the queue
 * and set the respective state var to on-off.
 *
 * RENDERING:
 * When the state of the spaceship change, we need to redraw the ship.
 * Hence, the rendering will be updated according to the state and ultimately
 * to the commands given by the player.
 */

namespace elements {

class Spaceship {
protected:
  /* Spaceship state: doMotion() will update this variables at each step*/
  float m_px, m_py, m_pz, m_facing;   // position
  float m_steering, m_steer_flight;   // internal state
  float m_speedX, m_speedY, m_speedZ; // velocity

  // Spaceship Stats: this will be constant over time
  float m_steer_speed, m_steer_return, m_grip, m_frictionX, m_frictionY,
      m_frictionZ, m_max_acceleration;
  float m_max_flight_acc;

  float m_scaleX, m_scaleY, m_scaleZ; // dimension scaling
  size_t m_rotation_angle; // depending on how the mesh has been designed
  agl::Vec3 m_viewUP,
      m_front_axis; // axis representing viewUP vector and front-facing vec

  // internal state of the spaceship. Each element represent a motion (on-off),
  // as described above.
  std::array<bool, spaceship::Motion::N_MOTION> m_state;

  // commands will be stored in a queue and processed with callbacks
  std::queue<spaceship::Command> m_cmds;

  agl::Env &m_env;
  agl::TexID m_tex;
  std::unique_ptr<agl::Mesh>
      m_mesh; // mesh structure for the Aventador Spaceship
  // angles, grip and friction

  // protected constructor to ensure singleton instance
  // instance is obtained through get_spaceship()
  Spaceship(const char *texture_filename, const char *mesh_filename);

  // drawing methods
  void draw() const;
  void drawFlicker() const;
  void drawHeadlight(float x, float y, float z, int lightN) const;

  // inner logic and physics of the spaceship
  bool get_state(spaceship::Motion mt);

  void processCommand();
  bool updateSteering();
  // these methods will differ between the Flappy 3D flight ship
  // and the normal one.
  virtual void updatePosition();
  virtual bool updateVelocity();
  virtual bool computePhysics();
  void doMotion();

public:
  friend std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                                  const char *mesh_filename,
                                                  bool m_flappy3D);

  // accessors
  inline float facing() const { return m_facing; }
  inline bool is_steering() const { return m_steering; }
  inline bool has_velocity() const { return m_speedZ; }
  inline float x() const { return m_px; }
  inline float y() const { return m_py; }
  inline float z() const { return m_pz; }

  inline void set_rotation_angle(size_t angle) { m_rotation_angle = angle; }
  inline void set_front_axis(agl::Vec3 axis) { m_front_axis = axis; }
  // reset spaceship status
  void init(bool truman = false);

  // APIs to interact with the spaceship
  void execute();
  void sendCommand(spaceship::Motion motion, bool on_off);
  void scale(float x, float y, float z);

  // render the Spaceship: TexID + Mesh
  virtual void render(bool flicker = false);
  void shadow();
};

class FlappyShip : Spaceship {
private:
  // override physics methods
  void updatePosition() override;
  bool updateVelocity() override;
  bool computePhysics() override;

  // methods for 3D flight only
  bool updateSteerFlight();

  FlappyShip(const char *texture_filename, const char *mesh_filename);

public:
  // get flappy ship instance
  friend std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                                  const char *mesh_filename,
                                                  bool m_flappy3D);

  void render(bool flicker = false) override;
};

std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                         const char *mesh_filename,
                                         bool m_flappy3D);

} // namespace elements

#endif // _SHIP_H