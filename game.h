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
  int m_camera_type;
  double m_start_time;    // fps value in the last interval
  double m_deadline_time; // fps currently drawn
  uint32_t m_last_time;

  agl::Env &m_env;
  std::unique_ptr<elements::Floor> m_floor;
  std::unique_ptr<elements::Sky> m_sky;
  std::unique_ptr<elements::Spaceship> m_ssh;

  void gameOnKey();
  void gameRender();
  void gameAction();

  void init();
  void setupObjectCamera();

  inline void change_camera_type() {
    m_camera_type = (m_camera_type + 1) % CAMERA_TYPE_MAX;
  }

public:
  void init();
  Game(int gameID); // constructor

  inline decltype(m_fps) fps() { return m_fps; }
  inline decltype(m_fps_now) fps_now() { return m_fps_now; }
  inline void incrementFPS() { m_fps_now++; }
};
}
#endif // GAME_H_
