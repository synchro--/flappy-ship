#ifndef UTILS_HXX

#define UTILS_HXX
//contains all the functions used to build the 3D-World and many other useful ones

#include <functional>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "types.hxx"
#include "gameState.hxx"
#include "car.hxx"

//accepts a lambda to be performed between between push and pop
//Saves time and helps against bugs
void scope(const std::function <void (void)>& lambda);

bool LoadTexture(int textbind, char *filename); 
void SetCoordToPixel(GameState gs);

//drawing functions  
void drawSphere(double r, int lats, int longs);
void drawSky(GameState gs); 
void drawFloor();
void drawAxis(); 
void redraw(); 


#endif