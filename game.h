#ifndef _GAME_H_
#define _GAME_H_

#include "agl.h"
#include "elements.h"
#include "types.h"

/*The logic of the game, putting all together*/

namespace game {

class Game {

private:
  // variables
  int m_gameID;
  State m_state;
  bool m_game_started;
  bool m_restart_game;
  int m_camera_type;
  double m_deadline_time; // fps currently drawn
  Uint32_t m_last_time;

  agl::Env &m_env;
  std::unique_ptr<elements::Floor> m_floor;
  std::unique_ptr<elements::Sky> m_sky;
  std::unique_ptr<elements::Spaceship> m_ssh;

  void gameOnKey(game::Key, bool pressed);
  void gameRender();
  void gameAction();
  void playGame();
  void changeState(game::State state);

  void init();
  void setupObjectCamera();

  inline void change_camera_type() {
    m_camera_type = (m_camera_type + 1) % CAMERA_TYPE_MAX;
  }

public:
  void init();
  Game(std::string gameID); // constructor
};
}
#endif // GAME_H_
