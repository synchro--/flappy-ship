#ifndef _ELEMENTS_HXX_
#define _ELEMENTS_HXX_

#include "agl.hxx"
#include "log.hxx"
#include "types.hxx"

/*
* elements.hxx describes the different objects (elements) of the game
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
}

// get singleton instance of floor
Floor *
get_floor(const char *filename);

// The Sky. Same mechanism as above
class Sky {
private:
  agl::Env &m_env;
  agl::TexID m_tex;
  double m_radius;
  int m_lats, m_longs;

  // construct the floor loading the texture
  Sky(const char *texture_filename, double radius);

public:
  // friend function to load the texture and create a singleton
  friend Sky *get_sky(const char *filename);

  void render();

  // accessors
  void set_params(double radius = 100.0, int lats = 20, int longs = 20);

  inline decltype(m_radius) radius(){return m_radius};
}

// get singleton instance of floor
Floor *
get_sky(const char *filename);

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
  agl::Env &m_env;
  agl::TexID m_tex;
  agl::Mesh m_mesh; // mesh structure for the Aventador Spaceship
  // angles, grip and friction

  // state
  bool m_state[5];

  // commands will be stored in a queue and processed with callbacks
  std::queue<spaceship::Command> m_cmds;

  // private constructor to ensure singleton instance
  // instance is obtained through get_spaceship()
  Spaceship(const char *texture_filename, const char *mesh_filename);

  // internal logic and physics of the spaceship

  bool updateNeeded();
  bool doMotion();

public:
  friend Spaceship *get_spaceship(const char *texture_filename,
                                  const char *mesh_filename);

  // APIs to interact with the spaceship
  void processCommand();
  void sendCommand(spaceship::Motion motion, bool on_off);
  // render the Spaceship: TexID + Mesh
  void render();
}
}

#endif //_ELEMENTS_HXX_
