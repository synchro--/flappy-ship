#ifndef UTILS_HXX

#define UTILS_HXX
// contains all the functions used to build the 3D-World and many other useful ones

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>

#include "car.hxx"
#include "game.hxx"
#include "types.hxx"

bool LoadTexture(int textbind, char *filename);
void SetCoordToPixel(Game gs);

// drawing functions
void drawSphere(double r, int lats, int longs);
void drawSky(Game gs);
void drawFloor();
void drawAxis();
void redraw();

#endif

// Switches mode into GL_MODELVIEW, and then loads an identity matrix.
void Context::setup_model() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Switches mode into GL_PERSPECTIVE, and then loads an identity matrix.
// The perspective is then arbitrarily set up.
void Context::setup_persp(float width, float height) {
  logs::v(__func__, "initializing perspective projection");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, width / height, .2, 1000.0);
}