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

  if (argc != 1) {
    lg::e(__func__, "Usage: ./game <player_name>");
    return EXIT_FAILURE;
  }
  lg::set_level(lg::Level::INFO);

  // std::string name(argv[1]);
  std::string name("Ciao");
  size_t num_rings = 2; // debug and dev stage still
  game::Game game(name, num_rings);
  game.run();

  // praying god everything's run ok
  return EXIT_SUCCESS;
}
