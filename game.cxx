#include "game.h"

namespace game {

Game::Game(std::string gameID)
    : m_gameID(gameID), m_state(State::GAME), m_camera_type(CAMERA_BACK_CAR),
      m_game_started(false), m_deadline_time(RING_TIME), m_last_time(.0),
      m_env(agl::get_env()), m_floor(nullptr), m_sky(nullptr), m_ssh(nullptr) 
      {}

/*
* Init the game:
* 1. Obtain main window from the environment
* 2. Load textures and mesh
*/
void Game::init() {
  // changeState(game::Splash);
  std::string win_name = "Main Window";
  m_main_win = m_env.createWindow(win_name, 0, 0, 800, 600);
  m_main_win->show();

  m_ssh = elements::get_spaceship("envmap_flipped.jpg", "Envos.obj");
  m_floor = elements::get_floor("nrm.tga");
  m_sky = elements::get_sky("sky_ok.jpg");
}

void Game::changeState(game::State state) {

  static const auto TAG = __func__; 

  if (state == m_state)
    return;

  if (m_state == State::GAME && state == State::SPLASH) {
    lg::e(TAG, "Can't go back to Splash while playing!");
    return;
  }

  if (m_state == State::SPLASH && state == State::END) {
    lg::e(TAG, "Can't go from Splash screen directly to the end. You can't skip to "
          "the conclusion..");
  }

  // dalla fine al menu pure non si può fare, da aggiungere

  // change state and callback functions
  if (state == State::MENU && m_state == State::GAME) {
    m_state = state;
  //  openMenu();
  }

  if (state == State::GAME && m_state == State::MENU) {
    m_state = state;
    playGame();
  }

  if (state == State::END && m_state == State::GAME) {
    m_state = state;
   // gameOver();
  }

  // shoudln't arrive here
  m_state = state;
}

void Game::gameAction() {
  // Azioni del gioco:
  // passano i secondi --> i frame sono da far mostrare ad Env
  m_ssh->execute();

  auto time_now = m_env.getTicks();
  m_deadline_time -= time_now - m_last_time;

  if (m_deadline_time < 0) { // let's leave a last second hope
    changeState(State::END);
  }

  // check se gli anelli sono stati attraversati
  // spawn nuovo anello + bonus time || crea porta finale (time diventa rosso)
  bool ring_crossed = true;
  if (ring_crossed) {
    m_deadline_time += game::BONUS_TIME;
  }
}

void Game::gameOnKey(Key key, bool pressed) {
  using namespace spaceship;
  using spaceship::Motion;
  Motion mt;
  bool game_triggered = false;

  switch (key) {
  case Key::W:
    mt = spaceship::Motion::THROTTLE;
    game_triggered = true;

  case Key::A:
    mt = spaceship::Motion::STEER_L;
    game_triggered = true;

  case Key::S:
    mt = spaceship::Motion::BRAKE;
    game_triggered = true;

  case Key::D:
    mt = spaceship::Motion::STEER_R;
    game_triggered = true;

  case Key::ESC:
    if (pressed) {
      changeState(State::MENU);
    }

  // environment change handlers!
  // Only on key down
  case Key::F1:
    if (pressed) {
      change_camera_type();
    }

  case Key::F2:
    if (pressed) {
      m_env.toggle_wireframe();
    }

  case Key::F3:
    if (pressed) {
      m_env.toggle_envmap();
    }

  case Key::F4:
    if (pressed) {
      m_env.toggle_headlight();
    }

  case Key::F5:
    if (pressed) {
      m_env.toggle_shadow();
    }

  default:
    break;
  }

  if (game_triggered) {
    if (!m_game_started) {
      m_last_time = m_env.getTicks();
      //     m_deadline_time = game::RING_TIME;
    }
  }

  m_ssh->sendCommand(mt, pressed);
}

/* Esegue il Rendering della scena */
void Game::gameRender() {

  m_env.lineWidth(3.0);
  // settiamo il viewport
  // glViewport(0, 0, scrW, scrH); //-- DENTRO WINDOW
  m_main_win->setupViewport();

  m_env.clearBuffer();

  m_env.setupPersp();
  m_env.setupModel();
  m_env.setupLightPosition();
  m_env.setupModelLights(); 

  setupShipCamera();

  m_floor->render();
  m_sky->render();
  m_ssh->render(m_env.isWireframe(), m_env.isHeadlight());

  // m_env.setCoordToPixel(); //serve??

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate
  // DA METTERE DENTRO WINDOW!!!!! <---------


  // disegnamo i fps (frame x sec) come una barra a sinistra.
  // (vuota = 0 fps, piena = 100 fps)

  // questa è una funzione di env

  // fa parte del HUD
  /*
  glBegin(GL_QUADS);
  float y = m_screenH * m_fps / 100;
  float ramp = m_fps / 100;
  glColor3f(1 - ramp, 0, ramp);
  glVertex2d(10, 0);
  glVertex2d(10, y);
  glVertex2d(0, y);
  glVertex2d(0, 0);
  glEnd();
  */

  m_env.enableLighting(); 

  m_main_win->refresh();
}

/*
* Bind all the callbacks of the environment to the corresponding
* functions of the GAME state.
*/
void Game::playGame() {
  // Using placeholders to perform currying, together with std::bind()
  // Each placeholder (_1, _2, etc.) represents a future argument that will be
  // passed
  // to the callback handler.
  using namespace std::placeholders;

  m_env.set_winevent_handler(std::bind(&Game::gameRender, this));
  m_env.set_render(std::bind(&Game::gameRender, this));
  m_env.set_action(std::bind(&Game::gameAction, this));

  m_env.set_keydown_handler(std::bind(&Game::gameOnKey, this, _1, true));
  m_env.set_keyup_handler(std::bind(&Game::gameOnKey, this, _1, false));
}

/*
* Run the game.
* 1. Init; 2. Splash screen; 3. Main event loop
*/
void Game::run() {
  init();

  // splash();

  m_env.mainLoop();
}

void Game::setupShipCamera() {
  double angle = m_ssh->facing();
  double cosf = cos(angle * M_PI / 180.0);
  double sinf = sin(angle * M_PI / 180.0);
  double cam_d, cam_h, eye_x, eye_y, eye_z, cen_x, cen_y, cen_z;
  double cosff, sinff;

  float px = m_ssh->x();
  float py = m_ssh->y();
  float pz = m_ssh->z();

  // controllo la posizione della camera a seconda dell'opzione selezionata
  switch (m_camera_type) {

  case CAMERA_BACK_CAR:
    cam_d = 2.5;
    cam_h = 1.0;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
    break;

  case CAMERA_TOP_FIXED:
    cam_d = 0.5;
    cam_h = 0.55;
    angle = m_ssh->facing() + 40.0;
    cosff = cos(angle * M_PI / 180.0);
    sinff = sin(angle * M_PI / 180.0);
    eye_x = px + cam_d * sinff;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosff;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
    break;

  case CAMERA_TOP_CAR:
    cam_d = 2.5;
    cam_h = 1.0;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
    break;

  case CAMERA_PILOT:
    cam_d = 0.2;
    cam_h = 0.55;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
    break;

  case CAMERA_MOUSE:
    m_env.translate(0, 0, -(m_env.eyeDist()));
    m_ssh->rotateView();
    /*
    lg::i("%f %f %f\n",view_alpha,view_beta,eyeDist);
                    eye_x=eyeDist*cos(view_alpha)*sin(view_beta);
                    eye_y=eyeDist*sin(view_alpha)*sin(view_beta);
                    eye_z=eyeDist*cos(view_beta);
                    cen_x = px - cam_d*sinf;
                    cen_y = py + cam_h;
                    cen_z = pz - cam_d*cosf;
                    gluLookAt(eye_x,eye_y,eye_z,cen_x,cen_y,cen_z,0.0,1.0,0.0);
    */
    break;
  }
}
}
