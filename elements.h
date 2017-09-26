#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#include "types.h"

#include "agl.h"
#include "log.h"

/*
 * elementsh describes the different objects (elements) of the game
 * Floor, Sky, ..., it will be expanded as the game gets more complex
 * and new elements gets added.
 */

namespace elements {

// utility function, returns the string related to the motion
const std::string motion_to_str(spaceship::Motion mt);

/*
 * The floor.
 * Construcor loads the texture and stores it in m_tex
 * Render function draw this tex
 * get_floor returns a singleton instance. This behaviour can be changed
 * in case we want to expand the game and get more instances of floors (or
 * planes).
 */

class Floor {
private:
  float m_size, m_height;
  agl::Env &m_env;  // reference to env, needed in the constructor
  agl::TexID m_tex; // floor texture

  // construct the floor loading the texture
  Floor(const char *texture_filename);

public:
  // friend function to load the texture and create a singleton
  friend Floor *get_floor(const char *filename);

  void render();
};

// get singleton instance of floor
Floor *get_floor(const char *filename);

// The Sky. Same mechanism as above
class Sky {
private:
  agl::Env &m_env;
  agl::TexID m_tex;
  double m_radius;
  int m_lats, m_longs;

  // construct the floor loading the texture
  Sky(const char *texture_filename);

public:
  // friend function to load the texture and create a singleton
  friend Sky *get_sky(const char *filename);

  void render();

  // accessors
  void set_params(double radius = 100.0, int lats = 20, int longs = 20);

  inline decltype(m_radius) radius() { return m_radius; }
};

// get singleton instance of sky
Sky *get_sky(const char *filename);

/*
 TODO class RING coming soon
 oppure trovare un'altra idea
 tipo paletti in cui si passa di fianco dalla direzione corretta
 e si fa quindi una sorta di slalom --> SpaceSlalom

 - come stabilire la direzione corretta e rilevera se si passa l'ostacolo dalla
 - parte giusta ? discutere con cillo
*/

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

class Spaceship {
private:
  /* Spaceship state: doMotion() will update this variables at each step*/
  float m_px, m_py, m_pz, m_facing;   // position
  float m_steering;                   // internal state
  float m_speedX, m_speedY, m_speedZ; // velocity

  // Spaceship Stats: this will be constant over time
  float m_steer_speed, m_steer_return, m_grip, m_frictionX,
      m_frictionY, m_frictionZ, m_max_acceleration;

  float m_scaleX, m_scaleY, m_scaleZ; // dimension scaling
  agl::Vec3 m_viewUP, m_front_axis; // axis representing viewUP vector and front-facing vec

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

  // private constructor to ensure singleton instance
  // instance is obtained through get_spaceship()
  Spaceship(const char *texture_filename, const char *mesh_filename);

  //drawing methods
  void draw() const;
  void drawHeadlight(float x, float y, float z, int lightN) const;

  // inner logic and physics of the spaceship
  bool get_state(spaceship::Motion mt);

  void init();
  void processCommand();
  void updateFly(); 
  void updatePosition(); 
  bool updateSteering();
  bool updateVelocity(); 

  bool computePhysics(); 
  void doMotion();

public:
  friend std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                                  const char *mesh_filename);

  // accessors
  inline float facing() const { return m_facing; }
  inline bool is_steering() const {return m_steering; }
  inline bool has_velocity() const {return m_speedZ; }
  inline float x() const { return m_px; }
  inline float y() const { return m_py; }
  inline float z() const { return m_pz; }

  // APIs to interact with the spaceship
  void execute();
  void sendCommand(spaceship::Motion motion, bool on_off);
  void scale(float x, float y, float z);

  // render the Spaceship: TexID + Mesh
  void render();
  void shadow();
  // rotate the view around the ship, to be used only on CAMERA_MOUSE mode
  void rotateView(int32_t view_alpha, int32_t view_beta);
};

std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                         const char *mesh_filename);
} // namespace elements

#endif //_ELEMENTS_H_
