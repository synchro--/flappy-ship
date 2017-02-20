#ifndef GAME_HXX
#define GAME_HXX
//The current state of the game with all the variables that need to be accessed globally

#include "types.hxx"
#include <SDL2/SDL.h>

class Game {
    public:
    void init(); 
    Game() { init(); } //constructor

    //define global vars
    //static, every class that inherits from Game share the same instance of the global vars
    static float viewAlpha, viewBeta, eyeDist;
    static int scrH, scrW;          // altezza e larghezza viewport (in pixels)
    static bool useWireframe, useEnvmap, useHeadlight, useShadow;
    static int cameraType;
    static float fps; // valore di fps dell'intervallo precedente
    static int fpsNow;    // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
    static int nstep; // numero di passi di FISICA fatti fin'ora

    static Uint32 timeLastInterval; // quando e' cominciato l'ultimo intervallo


};


#endif //GAME_HXX