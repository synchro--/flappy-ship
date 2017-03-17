#include <SDL2/SDL_ttf.h>
#include "agl.hxx"

using namespace agl; 

  // Returns the singleton instance of agl::Env, initializing it if necessary
  Env& get_env() {
  //having a unique ptr ensures the Env will be called only during the main

    //static std::unique_ptr<Context> s_env(nullptr);
    static auto s_env = std::make_unique<Env>();
    if (!s_env) {
      s_env.reset(new Env()); // initialize the Context on demand
    }

    return *s_env;
  }

class Env final {

public: 

void mat_scope(const std::function <void (void)> callback {
    glPushMatrix(); 
    callback(); 
    glPopMatrix();
}

bool LoadTexture(int textbind, char *filename) {
  SDL_Surface *s = IMG_Load(filename);
  if (!s)
    return false;

  glBindTexture(GL_TEXTURE_2D, textbind);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, s->w, s->h, GL_RGB, GL_UNSIGNED_BYTE,
                    s->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  return true;
}

void drawSphere(double r, int lats, int longs) {
  int i, j;
  for (i = 0; i <= lats; i++) {
    double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
    double z0 = sin(lat0);
    double zr0 = cos(lat0);

    double lat1 = M_PI * (-0.5 + (double)i / lats);
    double z1 = sin(lat1);
    double zr1 = cos(lat1);

    glBegin(GL_QUAD_STRIP);
    for (j = 0; j <= longs; j++) {
      double lng = 2 * M_PI * (double)(j - 1) / longs;
      double x = cos(lng);
      double y = sin(lng);

      // le normali servono per l'EnvMap
      glNormal3f(x * zr0, y * zr0, z0);
      glVertex3f(r * x * zr0, r * y * zr0, r * z0);
      glNormal3f(x * zr1, y * zr1, z1);
      glVertex3f(r * x * zr1, r * y * zr1, r * z1);
    }
    glEnd();
  }


void drawFloor() {
  const float S = 100; // size
  const float H = 0;   // altezza
  const int K = 150;   // disegna K x K quads

  // disegna KxK quads
  glBegin(GL_QUADS);
  glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
  glNormal3f(0, 1, 0);      // normale verticale uguale x tutti
  for (int x = 0; x < K; x++)
    for (int z = 0; z < K; z++) {
      float x0 = -S + 2 * (x + 0) * S / K;
      float x1 = -S + 2 * (x + 1) * S / K;
      float z0 = -S + 2 * (z + 0) * S / K;
      float z1 = -S + 2 * (z + 1) * S / K;
      glVertex3d(x0, H, z0);
      glVertex3d(x1, H, z0);
      glVertex3d(x1, H, z1);
      glVertex3d(x0, H, z1);
    }
  glEnd();
 }


}




}


// auto car = std::make_unique<Car>(); // requires C++14