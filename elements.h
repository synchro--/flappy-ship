#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#include "agl.h"
#include "log.h"
#include "types.h"

/*
* elementsh describes the different objects (elements) of the game
* Floor, Sky, ..., it will be expanded as the game gets more complex
* and new elements gets added.
*/

namespace elements {

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

  inline decltype(m_radius) radius(){return m_radius};
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
  // position
  float m_px, m_py, m_pz, m_facing, m_angle, m_steering, m_view_alpha,
      m_view_beta;
  // speed and movements
  float m_speedX, m_speedY, m_speedZ, m_steer_speed, m_steer_return_speed,
      m_grip, m_frictionX, m_frictionY, m_frictionZ, m_max_acceleration;
  // shape
  float m_scaleX, m_scaleY, m_scaleZ;

  // state
  bool m_state[5];

  // commands will be stored in a queue and processed with callbacks
  std::queue<spaceship::Command> m_cmds;

  agl::Env &m_env;
  agl::TexID m_tex;
  agl::Mesh m_mesh; // mesh structure for the Aventador Spaceship
  // angles, grip and friction

  // private constructor to ensure singleton instance
  // instance is obtained through get_spaceship()
  Spaceship(const char *texture_filename, const char *mesh_filename);

  // internal logic and physics of the spaceship

  bool draw(bool wireFrame_on = false, bool headlight_on = false) const;
  void drawHeadlight(float x, float y, float z, int lightN);
  bool doMotion();
  void processCommand();
  void shadow();

  /*bool updateNeeded();
  bool updatePhysics();
  bool updateSteering();
  bool updatePosition();*/

public:
  friend std::unique_ptr<Spaceship> get_spaceship(const char *texture_filename,
                                                  const char *mesh_filename);

  inline float angle() const { return m_angle; }
  inline float x() const { return m_px; }
  inline float y() const { return m_py; }
  inline float z() const { return m_pz; }
  inline float facing() const { return m_facing; }

  // APIs to interact with the spaceship
  bool execute();
  void sendCommand(spaceship::Motion motion, bool on_off);
  void setScale(float x, float y, float z);
  // render the Spaceship: TexID + Mesh
  void render(bool wireFrame_on = false, bool headlight_on = false) const;
  // rotate the view around the ship, to be used only on CAMERA_MOUSE mode
  void rotateView();
};
}

#endif //_ELEMENTS_H_
