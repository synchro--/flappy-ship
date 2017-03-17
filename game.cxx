#include "game.hxx"

void Game::init() {}

// initialize static variables
float Game::viewAlpha = 20;
float Game::viewBeta = 40; // angoli che definiscono la vista
float Game::eyeDist = 5.0; // distanza dell'occhio dall'origine
int Game::scrH = 750;
int Game::scrW = 750; // altezza e larghezza viewport (in pixels)
bool Game::useWireframe = false;
bool Game::useEnvmap = true;
bool Game::useHeadlight = false;
bool Game::useShadow = true;
int Game::cameraType = 0;
int Game::nstep = 0;
float Game::fps = 0;
int Game::fpsNow = 0;
Uint32 Game::timeLastInterval = 0;