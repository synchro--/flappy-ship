#ifndef _GAME_H_
#define _GAME_H_

#include "types.h"

#include "agl.h"
#include "coord_system.h"
#include "elements.h"

/*The logic of the game, putting all together*/

namespace game {

class Game final {

private:
  // variables
  State m_state;

  bool m_game_started;
  bool m_restart_game;
  bool m_victory;
  size_t m_cur_setting; // setting currently highlighted 

  // special var for 3D flight, it affects the whole game
  bool m_flappy3D;

  int m_camera_type;
  float m_eye_dist, m_view_alpha, m_view_beta;

  double m_deadline_time, m_player_time;
  uint32_t m_penalty_time, m_last_time;

  // environment
  agl::Env &m_env;
  std::unique_ptr<agl::SmartWindow> m_main_win;
  std::unique_ptr<agl::AGLTextRenderer> m_text_renderer;
  std::unique_ptr<agl::AGLTextRenderer> m_text_big;

  // various elements
  std::unique_ptr<elements::Spaceship> m_ssh;
  elements::Floor *m_floor;
  elements::Sky *m_sky;
  agl::TexID m_splash_tex, m_menu_tex, m_win_tex, m_lost_tex;
  std::vector<Setting> m_settings; 

  // Ring stuff
  std::vector<elements::Ring> m_rings;
  size_t m_num_rings;
  size_t m_cur_ring_index;

  // Cube stuff
  std::vector<elements::BadCube> m_cubes;
  size_t m_num_cubes;

  // methods
  void setupShipCamera();
  void changeState(game::State state);
  inline void change_camera_type() {
    m_camera_type = (m_camera_type + 1) % CAMERA_TYPE_MAX;
  }

  // Draw the Minimap with all the current rings
  void drawMiniMap();
  // Draw the HeadUP Display (FPS - Current Time Left - Ring crossed)
  void drawHUD();
  void drawRanking();
  void drawSettingOnOff(size_t Ycoord, Setting &sg, bool isSelected = false) const;
  void drawSettingItem(size_t Xcoord, size_t Yoord, const char *name, bool isSelected = false) const;
  void drawSplash();
  void updateRanking();

  void gameAction();
  void gameOnKey(game::Key, bool pressed);
  void gameOnMouse(MouseEvent ev, int32_t x, int32_t y = -1.0);
  void gameOver();
  void gameRender();
  void renderMenu();

  // callback setters: change handlers according to the state of the game
  void openSettings();
  void playGame();
  void splash();

  // initialization functions for rings, cubes and game
  void init_rings();
  void init_cubes();
  void init_settings(); 
  void init();

  void restartGame();

public:
  std::string m_gameID;

  Game(std::string gameID, size_t num_rings); // constructor
  void run();
};

} // namespace game
#endif // GAME_H_
