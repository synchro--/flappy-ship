#ifndef _GAME_H_
#define _GAME_H_

#include "types.h"

#include "agl.h"
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
  double m_deadline_time; // fps currently drawn
  uint32_t m_last_time;

  agl::Env &m_env;
  std::unique_ptr<agl::SmartWindow> m_main_win;
  std::unique_ptr<elements::Spaceship> m_ssh;
  elements::Floor *m_floor;
  elements::Sky *m_sky;

  void changeState(game::State state);
  void setupShipCamera();
  void gameOnKey(game::Key, bool pressed);
  void gameRender();
  void gameAction();
  void playGame();

  void init();
  void setupObjectCamera();

  inline void change_camera_type() {
    m_camera_type = (m_camera_type + 1) % CAMERA_TYPE_MAX;
  }

public:
  Game(std::string gameID); // constructor
  void run();
};
} // namespace game
#endif // GAME_H_
