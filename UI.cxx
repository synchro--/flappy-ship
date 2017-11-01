#include "game.h"
#include <algorithm>

/* This source contains all the Menu functions */

namespace game {

void Game::splash() {
  //  if this is set, then the splash will redraw itself at every cycle,
  //  pointless m_env.set_render(std::bind(&Game::drawSplash, this));
  m_env.set_winevent_handler(std::bind(&Game::drawSplash, this));

  // modify keydown handler with a small lambda
  // it's easy: if return is pressed then we enter the game
  m_env.set_keydown_handler([&](Key key) {
    if (key == Key::RETURN) {
      changeState(State::GAME);
    }
  });
}

// Load a texture image to be shown as a Splash screen
void Game::drawSplash() {
  std::string title = "Truman Escape";
  std::string subtitle = "Press Enter to start";
  const static auto X_O = m_main_win->m_width - 800;
  const static auto Y_O = m_main_win->m_height - 300;
  // draw texture and print title
  m_main_win->textureWindow(m_splash_tex);
  m_main_win->printOnScreen([&] {
    m_env.setColor(agl::WHITE);
    m_text_big->render(X_O, Y_O, title);
    m_text_renderer->render(X_O + 100, Y_O - 200, subtitle);
  });

  // refresh window
  m_main_win->refresh();
}

void Game::drawMiniMap() {
  // coords
  const auto X_O = m_main_win->m_width - 835;
  const auto Y_O = m_main_win->m_height - 500;

  // draw on pixel coords
  m_main_win->printOnScreen([&] {
    // draw circle
    float map_radius = 50.0f;
    float ratio = map_radius / 100.0f; // sky-to-minimap radius ratio
    int x_sign = -1;
    m_env.drawCircle(X_O, Y_O, map_radius);

    // draw spaceship dot
    float dot_radius = 3.0f;
    m_env.setColor(agl::BLACK);
    float ship_x = m_ssh->x() * ratio * x_sign;
    float ship_y = m_ssh->z() * ratio;
    m_env.drawCircle(X_O - ship_x, Y_O - ship_y, dot_radius);

    // draw ring dots
    for (size_t i = 0; i <= m_cur_ring_index; ++i) {
      if (m_cur_ring_index >= m_num_rings) {
        break;
      }
      auto ring = m_rings.at(i);
      m_env.setColor(ring.isTriggered() ? agl::RED : agl::GREEN);
      float ring_x = ring.x() * ratio * x_sign;
      float ring_y = ring.z() * ratio;
      m_env.drawCircle(X_O - ring_x, Y_O - ring_y, dot_radius);
    }

    // draw badcubes dots
    for (size_t i = 0; i < m_num_cubes; ++i) {
      auto cube = m_cubes.at(i);
      m_env.setColor(agl::YELLOW);
      float cube_x = cube.x() * ratio * x_sign;
      float cube_y = cube.z() * ratio;
      m_env.drawCircle(X_O - cube_x, Y_O - cube_y, dot_radius - 1.0f);
    }
  });
}

// draw a simple HeadUP Display
void Game::drawHUD() {
  auto fps = m_env.get_fps();
  const auto X_O = m_main_win->m_width - 850;
  const auto Y_O = m_main_win->m_height - 50;
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
  drawMiniMap();
}

// read ranking.txt file, sort players time and log it
void Game::updateRanking() {
  // gather ranking data
  auto entries = lg::readRankingData("ranking.txt");
  // sort it with the time of the new winner and log new ranking
  Entry new_entry(m_gameID, m_player_time / 1000.0);
  entries.push_back(new_entry);
  // sort with a simple lambda
  std::sort(entries.begin(), entries.end(), [](const Entry e1, const Entry e2) {
    return e1.second < e2.second;
  });
  // log ranking
  lg::logRanking("ranking.txt", entries);
}

// update ranking and set the proper callbacks
void Game::gameOver() {
  if (m_victory) {
    updateRanking();
  }

  // reset handlers that must NOT be used
  m_env.set_keyup_handler();
  m_env.set_action();
  m_env.set_mouse_handler();
  // set proper callbacks
  m_env.set_render(std::bind(&Game::drawRanking, this));
  m_env.set_winevent_handler(std::bind(&Game::drawRanking, this));

  // modify keydown handler with a small lambda
  // it's easy: if return is pressed then we enter the game
  m_env.set_keydown_handler([&](Key key) {
    if (key == Key::RETURN) {
      // changeState(State::SPLASH);
      m_env.quitLoop();
    }
  });
}

// log ranking time and print it to screen
void Game::drawRanking() {
  const static auto X_O = m_main_win->m_width * 0.25;
  const static auto Y_O = m_main_win->m_height - 100;
  const static auto OFFSET = 370;
  const static auto INTERVAL = m_text_renderer->get_height() + 5;
  const static auto MAX_ENTRIES = 5;

  // draw texture and print title
  m_main_win->colorWindow(m_victory ? agl::GREEN : agl::RED);
  m_main_win->printOnScreen([&] {
    m_env.setColor(agl::WHITE);
    std::string title = m_victory ? "YOU WIN" : "YOU LOSE";
    m_text_big->render(X_O, Y_O, title);

    // print Ranking on screen
    auto entries = lg::readRankingData("ranking.txt");
    size_t todo = MAX_ENTRIES < entries.size() ? MAX_ENTRIES : entries.size();
    for (size_t i = 0; i < todo; ++i) {
      m_text_renderer->render(X_O - 100, Y_O - 100, "RANKING:");
      m_text_renderer->render(X_O + 100, Y_O - OFFSET + (i * INTERVAL),
                              entries.at(i).first); // name
      m_text_renderer->renderf(X_O + 200, Y_O - OFFSET + (i * INTERVAL),
                               ": %2.2f", entries.at(i).second); // time
    }

    // restart or quit
    m_text_renderer->render(X_O - 100, 150, "restart");
    m_text_renderer->render(780, 150, "quit");
  });

  // refresh window
  m_main_win->refresh();
}

} // namespace game
