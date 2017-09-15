#ifndef _GAME_H_
#define _GAME_H_

#include "agl.h"
#include "types.h"

/*The logic of the game, putting all together*/

namespace game {

class Game {

private:
  agl::Env m_env;
  // variables

  void gameOnKey();
  void gameRender();
  void gameAction();

public:
  void init();
  Game() { init(); } // constructor
};
}
#endif // GAME_H_
