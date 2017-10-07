#ifndef _GAME_H_
#define _GAME_H_

#include "types.h"

#include "agl.h"
#include "coord_system.h"
#include "elements.h"

/*The logic of the game, putting all together*/

namespace game {

class Game final {

private:
  // variables
  std::string m_gameID;
  State m_state;
  bool m_game_started;
  bool m_restart_game;
  int m_camera_type;
  float m_eye_dist, m_view_alpha, m_view_beta;
  double m_deadline_time; // fps currently drawn
  uint32_t m_penalty_time; 
  uint32_t m_last_time;

  agl::Env &m_env;
  std::unique_ptr<agl::SmartWindow> m_main_win;
  std::unique_ptr<elements::Spaceship> m_ssh;
  elements::Floor *m_floor;
  elements::Sky *m_sky;
  // Ring stuff
  std::vector<elements::Ring> m_rings;
  size_t m_num_rings;
  size_t m_cur_ring_index;

  // Cube stuff 
  std::vector<elements::BadCube> m_cubes; 
  size_t m_num_cubes; 

  void setupShipCamera();
  void changeState(game::State state);
  void gameAction();
  void gameOnKey(game::Key, bool pressed);
  void gameOnMouse(MouseEvent ev, int32_t x, int32_t y = -1.0);
  void gameRender();
  // set the callbacks to the game functions
  void playGame();

  void init_rings();
  void init_cubes(); 
  void init();

  inline void change_camera_type() {
    m_camera_type = (m_camera_type + 1) % CAMERA_TYPE_MAX;
  }

public:
  Game(std::string gameID, size_t num_rings); // constructor
  void run();
};
} // namespace game
#endif // GAME_H_
