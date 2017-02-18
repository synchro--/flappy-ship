#include "gameState.hxx"

void GameState::init() {
  
}

//initialize static variables
float GameState::viewAlpha = 20;
float GameState::viewBeta = 40; // angoli che definiscono la vista
float GameState::eyeDist = 5.0;                 // distanza dell'occhio dall'origine
int GameState::scrH = 750;
int GameState::scrW = 750;          // altezza e larghezza viewport (in pixels)
bool GameState::useWireframe = false;
bool GameState::useEnvmap = true;
bool GameState::useHeadlight = false;
bool GameState::useShadow = true;
int GameState::cameraType = 0;
int GameState::nstep =0; 
float GameState::fps =0; 
int GameState::fpsNow = 0; 
Uint32 GameState::timeLastInterval = 0;