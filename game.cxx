#include "game.h"
#include "random"

namespace game {

Game::Game(std::string gameID, size_t num_rings)
    : m_gameID(gameID), m_state(State::SPLASH), m_camera_type(CAMERA_BACK_CAR),
      m_eye_dist(5.0), m_view_alpha(20.0), m_view_beta(40.0), m_victory(false),
      m_game_started(false), m_deadline_time(0.0), m_last_time(.0),
      m_penalty_time(0.0), m_num_rings(num_rings), m_cur_ring_index(0),
      m_env(agl::get_env()), m_num_cubes(10), m_main_win(nullptr),
      m_floor(nullptr), m_sky(nullptr), m_ssh(nullptr) {}

/*
 * Init the game:
 * 1. Obtain main window from the environment
 * 2. Load textures and mesh
 */
void Game::init() {
  std::string win_name = "Main Window";
  m_main_win = m_env.createWindow(win_name, 0, 0, m_env.get_win_width(),
                                  m_env.get_win_height());
  m_main_win->show();
  m_floor = elements::get_floor("Texture/tex1.jpg");
  m_sky = elements::get_sky("Texture/space1.jpg");
  m_ssh =
      elements::get_spaceship("Texture/envmap_flipped.jpg", "Mesh/Envos.obj");
  m_text_renderer = agl::getTextRenderer("Fonts/neuropol.ttf", 30);
  m_text_big = agl::getTextRenderer("Fonts/neuropol.ttf", 72);
  m_splash_tex = m_env.loadTexture("Texture/splash2.jpg");

  m_ssh->scale(spaceship::ENVOS_SCALE, spaceship::ENVOS_SCALE,
               spaceship::ENVOS_SCALE);

  init_rings();
  init_cubes();
}

void Game::changeState(game::State next_state) {

  static const auto TAG = __func__;

  if (next_state == m_state)
    return;

  switch (next_state) {
   case State::SPLASH:
    if ((!m_game_started) || (m_state == State::MENU && m_restart_game)) {
      m_state = next_state;
      splash();
    }
    break;

   case State::MENU: 
    if(m_state == State::GAME) {
    m_state = next_state;
    // gameMenu();
  } break;

   case State::GAME:
    if (m_state == State::SPLASH || m_state == State::MENU) {
      m_state = next_state;
      playGame();
    }
    break;

   case State::END:
    if (m_state == State::GAME) {
      m_state = next_state;
      // gameOver();
    }

  default:
    // shoudln't arrive here
    lg::e(TAG, "Game status not recognized");
  }
}

// draw a simple HeadUP Display
void Game::drawHUD() {
  auto fps = m_env.get_fps();
  auto X_O = m_main_win->m_width - 850;
  auto Y_O = m_main_win->m_height - 50;
  const static auto offset = 280;

  // draw data on the window
  m_main_win->printOnScreen([&] {
    m_env.setColor(agl::WHITE);
    m_text_renderer->renderf(X_O, Y_O, "FPS:%2.1f", fps);
    m_text_renderer->renderf(X_O + offset, Y_O, "TIME:%2.1fS",
                             (m_deadline_time / 1000.0));
    m_text_renderer->renderf(X_O + 2 * offset, Y_O, "RINGS: %d/%d",
                             m_cur_ring_index, m_num_rings);
  });

  // draw minimap
  // drawMinimap();
}

void Game::gameAction() {
  // Game actions:
  // - Ship execute a step of physics
  // - time pass by
  // - if time is < 0 game over
  // - if crosses ring: get bonus time
  // - if ring is last one: final gate
  // - if crosses final gate: WIN!

  m_ssh->execute();

  // only if game has started, i.e. a key has been pressed
  if (m_game_started) {
    auto time_now = m_env.getTicks();
    auto diff = time_now - m_last_time;
    m_deadline_time -= diff;
    // if a penalty has been triggered, compute its remaining time
    m_penalty_time = m_penalty_time > 0.0 ? (m_penalty_time - 100) : 0.0;

    lg::i(__func__, "Time left: %f %f", (m_deadline_time / 1000.0),
          m_penalty_time / 1000.0);
    m_last_time = time_now;

    if (m_deadline_time < 0) { // let's leave a last second hope
      m_victory = false;
      // changeState(State::END);
    }
  }

  auto &current_ring = m_rings[m_cur_ring_index];
  // check se gli anelli sono stati attraversati
  // spawn nuovo anello + bonus time || crea porta finale (time diventa rosso)
  current_ring.checkCrossing(m_ssh->x(), m_ssh->z());
  bool ring_crossed;
  ring_crossed = current_ring.isTriggered();

  if (ring_crossed) {
    m_deadline_time += game::RING_TIME;
    if (++m_cur_ring_index == m_num_rings) {
      lg::i(__func__, "GAME END!!");
      m_victory = true;
      // changeState(State::END);
    }
  }

  for (auto &cube : m_cubes) {
    if (cube.checkCrossing(m_ssh->x(), m_ssh->z())) {
      lg::i(__func__, "Penalty!");
      m_penalty_time = 6000U;
      break;
    }
  }
}


void Game::init_rings() {
  m_rings.clear();

  // generate coordinate to place the rings using the coordinate generator
  // see coord_system.h
  // todo: add a check on minimum distance between each of the rings
  for (size_t i = 0; i < m_num_rings; ++i) {
    auto coords = coordinateGenerator::randomCoord2D();
    float y = 1.5; // height of the ring
    m_rings.emplace_back(coords.first, y, coords.second);
  }
}

void Game::init_cubes() {
  // cubes
  m_cubes.clear();
  for (size_t i = 0; i < m_num_cubes; ++i) {
    auto coords = coordinateGenerator::randomCoord2D();
    float y = 2.5; // height of the ring
    m_cubes.emplace_back(coords.first, y, coords.second);
  }
}

void Game::gameOnKey(Key key, bool pressed) {
  bool trig_motion = false;
  spaceship::Motion mt;

  switch (key) {

  case Key::W:
    mt = spaceship::Motion::THROTTLE;
    trig_motion = true;
    break;

  case Key::A:
    mt = spaceship::Motion::STEER_L;
    trig_motion = true;
    break;

  case Key::S:
    mt = spaceship::Motion::BRAKE;
    trig_motion = true;
    break;

  case Key::D:
    mt = spaceship::Motion::STEER_R;
    trig_motion = true;
    break;

  case Key::ESC:
    if (pressed) {
      changeState(State::MENU);
    }
    break;

  // environment change handlers!
  // Only on key down
  case Key::F1:
    if (pressed) {
      lg::i(__func__, "Changing camera");
      change_camera_type();
    }
    break;

  case Key::F2:
    if (pressed) {
      m_env.toggle_wireframe();
    }
    break;

  case Key::F3:
    if (pressed) {
      m_env.toggle_envmap();
    }
    break;

  case Key::F4:
    if (pressed) {
      m_env.toggle_headlight();
    }
    break;

  case Key::F5:
    if (pressed) {
      m_env.toggle_shadow();
    }
    break;

  default:
    break;
  }

  // send a command to the spaceship only if triggered
  if (trig_motion) {
    if (!m_game_started) {
      m_game_started = true;
      m_last_time = m_env.getTicks();
      m_deadline_time = game::RING_TIME;
    }

    m_ssh->sendCommand(mt, pressed);
  }
}

// Handler for mouse events, like zooming and perspective change
// Values changed with this method will only take effect when setting up
// the camera in setupShipCamera()
// Used only when camera is set to CAMERA_MOUSE
void Game::gameOnMouse(MouseEvent ev, int32_t x, int32_t y) {
  // Process mouse events only in State GAME
  if (m_state == State::GAME) {
    switch (ev) {
    case MouseEvent::MOTION: {
      m_view_alpha = y;
      m_view_beta = x;

      // avoid ending up under the ship
      m_view_beta = (m_view_beta < 5) ? 5 : m_view_beta;
      m_view_beta = (m_view_beta > 90) ? 90 : m_view_beta;
    } break;

    case MouseEvent::WHEEL: {
      // if mouse event is a wheel roll, only the first value is significant
      if (x < 0) {
        // zoom in
        m_eye_dist = m_eye_dist * 0.9;
        // can't be < 1
        m_eye_dist = m_eye_dist < 1 ? 1 : m_eye_dist;
      }

      else if (x > 0) {
        // Zoom out
        m_eye_dist = m_eye_dist / 0.9;
      }
    } break;

    default:
      break;
    }
  }
}

/* Esegue il Rendering della scena */
void Game::gameRender() {

  m_env.lineWidth(3.0);
  // remember to setup the viewport
  m_main_win->setupViewport();
  // buffer - lighting - perspective setup
  m_env.clearBuffer();
  m_env.disableLighting();
  m_env.setupPersp();
  m_env.setupModel();
  m_env.setupLightPosition();
  m_env.setupModelLights();
  // update camera
  setupShipCamera();

  // Render all elements
  m_floor->render();
  m_sky->render();

  // FLICKERING PENALTY
  // if the spaceship hit a cube it will be rendered in a flickered way
  // switching from goroud to wirefram rend every 200ms
  if (m_penalty_time && ((m_penalty_time / 200) % 2 == 1)) {
    m_ssh->render(true);
  } else {
    m_ssh->render();
  }

  // rings: render till the first ring that's not triggered yet
  for (size_t i = 0; i < m_num_rings; ++i) {
    auto &ring = m_rings[i];
    ring.render();

    if (!ring.isTriggered()) {
      break;
    }
  }

  // render all BadCubes. They'll be an obstacle from the beginning
  for (auto &cube : m_cubes) {
    cube.render();
  }
  // apply shadow
  if (m_env.isShadow()) {
    m_ssh->shadow();
  }

  drawHUD();

  m_env.enableLighting();

  // refresh the view
  m_main_win->refresh();
}

/*
 * Bind all the callbacks of the environment to the corresponding
 * functions of the GAME state.
 */
void Game::playGame() {
  // Using placeholders to perform currying, together with std::bind()
  // Each placeholder (_1, _2, etc.) represents a future argument that will be
  // passed to the callback handler.
  using namespace std::placeholders;

  m_env.set_winevent_handler(std::bind(&Game::gameRender, this));
  m_env.set_render(std::bind(&Game::gameRender, this));
  m_env.set_action(std::bind(&Game::gameAction, this));

  m_env.set_keydown_handler(std::bind(&Game::gameOnKey, this, _1, true));
  m_env.set_keyup_handler(std::bind(&Game::gameOnKey, this, _1, false));
  m_env.set_mouse_handler(std::bind(&Game::gameOnMouse, this, _1, _2, _3));
}

/*
 * Run the game.
 * 1. Init; 2. Splash screen; 3. Main event loop
 */
void Game::run() {
  init();

  splash();
  //playGame();

  m_env.renderLoop();
}

void Game::setupShipCamera() {
  // angle of the SShip wrt to X-axis in the x0z plane
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

  case CAMERA_BACK_CAR: {
    cam_d = 2.3;
    cam_h = 1.0;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
  } break;

  case CAMERA_TOP_FIXED: {
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
  } break;

  case CAMERA_TOP_CAR: {
    cam_d = 6.5;
    cam_h = 3.0;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
  } break;

  case CAMERA_PILOT: {
    cam_d = 1.0;
    cam_h = 1.05;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);
  } break;

  case CAMERA_MOUSE: {
    cam_d = 2.3;
    cam_h = 1.0;
    eye_x = px + cam_d * sinf;
    eye_y = py + cam_h;
    eye_z = pz + cam_d * cosf;
    cen_x = px - cam_d * sinf;
    cen_y = py + cam_h;
    cen_z = pz - cam_d * cosf;
    m_env.setCamera(eye_x, eye_y, eye_z, cen_x, cen_y, cen_z, 0.0, 1.0, 0.0);

    const auto axisX = agl::Vec3(1, 0, 0);
    const auto axisY = agl::Vec3(0, 1, 0);

    m_env.translate(0, 0, m_eye_dist);
    m_env.rotate(m_view_beta, axisX);
    m_env.rotate(m_view_alpha, axisY);

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
  } break;

  default:
    break;
  }
}
} // namespace game
