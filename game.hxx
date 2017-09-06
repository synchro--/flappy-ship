#ifndef _GAME_HXX
#define _GAME_HXX

#include "types.hxx"
#include <SDL2/SDL.h>

/*The logic of the game, putting together all the pieces*/

class Game {
public:
  void init();
  Game() { init(); } // constructor

  // define global vars
  //, every class that inherits from Game share the same instance of the global
  // vars
  float view_alpha, view_beta, eyeDist;
  int scrH, scrW; // altezza e larghezza viewport (in pixels)
  bool useWireframe, useEnvmap, useHeadlight, useShadow;
  int cameraType;
  float fps;  // valore di fps dell'intervallo precedente
  int fpsNow; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
  int nstep;  // numero di passi di FISICA fatti fin'ora

  Uint32 timeLastInterval; // quando e' cominciato l'ultimo intervallo
};

#endif // GAME_HXX