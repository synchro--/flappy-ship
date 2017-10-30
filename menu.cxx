#include "game.h"

/* This source contains all the Menu functions */

namespace game {

void Game::splash() {
  m_env.set_render(std::bind(&Game::drawSplash, this));
  m_env.set_winevent_handler(std::bind(&Game::drawSplash, this));

  // modify keydown handler
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

} // namespace game