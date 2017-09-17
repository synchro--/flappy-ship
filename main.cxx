#include <cmath>

#include "agl.h"
#include "elements.h"
#include "game.h"
#include "types.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main(int argc, char **argv) {

  if (argc != 2) {
    lg::e("Usage: ./game <player_name>");
    return EXIT_FAILURE;
  }
  lg::set_level(lg::Level::INFO);

  std::string name(argv[1]);
  game::Game game(name);
  game.run();

  // praying god everything's run ok
  return EXIT_SUCCESS;
}
