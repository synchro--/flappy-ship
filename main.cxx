#include "game.hh"
#include "log.hh"

#include <iostream>
#include <cstdlib>


int main(void) {
    lg::set_level(lg::Level::INFO); 

    game::Game game(); 
    game.run(); 

    //praying god everything's run ok
    return EXIT_SUCCESS; 
}