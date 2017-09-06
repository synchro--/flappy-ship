#include "utils.hxx"

// accepts a lambda to be performed between between push and pop
// Saves time and helps against bugs
void mat_scope(const std::function<void(void)> &lambda) {
  glPushMatrix();
  lambda();
  glPopMatrix();
}

// load texture from an image and return bool if success
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

// disegna gli assi nel sist. di riferimento
void drawAxis() {
  const float K = 0.10;
  glColor3f(0, 0, 1);
  glBegin(GL_LINES);
  glVertex3f(-1, 0, 0);
  glVertex3f(+1, 0, 0);

  glVertex3f(0, -1, 0);
  glVertex3f(0, +1, 0);

  glVertex3f(0, 0, -1); 

  glVertex3f(0, 0, +1);
  glEnd();

  glBegin(GL_TRIANGLES);
  glVertex3f(0, +1, 0);
  glVertex3f(K, +1 - K, 0);
  glVertex3f(-K, +1 - K, 0);

  glVertex3f(+1, 0, 0);
  glVertex3f(+1 - K, +K, 0);
  glVertex3f(+1 - K, -K, 0);
  
  glVertex3f(0, 0, +1);
  glVertex3f(0, +K, +1 - K);
  glVertex3f(0, -K, +1 - K);
  glEnd();
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
}

void drawFloor() {
  const float S = 100; // size
  const float H = 0;   // altezza
  const int K = 150;   // disegna K x K quads

  /*
  //vecchio codice ora commentato
  // disegna un quad solo
  glBegin(GL_QUADS);
    glColor3f(0.5, 0.2, 0.0);
    glNormal3f(0,1,0);
    glVertex3d(-S, H, -S);
    glVertex3d(+S, H, -S);
    glVertex3d(+S, H, +S);
    glVertex3d(-S, H, +S);
  glEnd();
  */

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

void drawSky(Game gs) {

  if (gs.useWireframe) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0, 0, 0);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawSphere(100.0, 20, 20);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(1, 1, 1);
    glEnable(GL_LIGHTING);
  } else {
    glBindTexture(GL_TEXTURE_2D, 2);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); // Env map
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);

    //   drawCubeFill();
    drawSphere(100.0, 20, 20);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
  }
}

void redraw() {
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type = SDL_WINDOWEVENT;
  e.window.event = SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schemo
void SetCoordToPixel(Game gs) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1, -1, 0);
  glScalef(2.0 / gs.scrW, 2.0 / gs.scrH, 1);
}

/*
//vecchio codice ora commentato
// disegna un cubo rasterizzando quads
void drawCubeFill()
{
const float S=100;

  glBegin(GL_QUADS);

    glNormal3f(  0,0,+1  );
    glVertex3f( +S,+S,+S );
    glVertex3f( -S,+S,+S );
    glVertex3f( -S,-S,+S );
    glVertex3f( +S,-S,+S );

    glNormal3f(  0,0,-1  );
    glVertex3f( +S,-S,-S );
    glVertex3f( -S,-S,-S );
    glVertex3f( -S,+S,-S );
    glVertex3f( +S,+S,-S );

    glNormal3f(  0,+1,0  );
    glVertex3f( +S,+S,+S );
    glVertex3f( -S,+S,+S );
    glVertex3f( -S,+S,-S );
    glVertex3f( +S,+S,-S );

    glNormal3f(  0,-1,0  );
    glVertex3f( +S,-S,-S );
    glVertex3f( -S,-S,-S );
    glVertex3f( -S,-S,+S );
    glVertex3f( +S,-S,+S );

    glNormal3f( +1,0,0  );
    glVertex3f( +S,+S,+S );
    glVertex3f( +S,-S,+S );
    glVertex3f( +S,-S,-S );
    glVertex3f( +S,+S,-S );

    glNormal3f( -1,0,0  );
    glVertex3f( -S,+S,-S );
    glVertex3f( -S,-S,-S );
    glVertex3f( -S,-S,+S );
    glVertex3f( -S,+S,+S );

  glEnd();
}

// disegna un cubo in wireframe
void drawCubeWire()
{
  glBegin(GL_LINE_LOOP); // faccia z=+1
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );
  glEnd();

  glBegin(GL_LINE_LOOP); // faccia z=-1
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );
  glEnd();

  glBegin(GL_LINES);  // 4 segmenti da -z a +z
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );

    glVertex3f( +1,-1,-1 );
    glVertex3f( +1,-1,+1 );

    glVertex3f( +1,+1,-1 );
    glVertex3f( +1,+1,+1 );

    glVertex3f( -1,+1,-1 );
    glVertex3f( -1,+1,+1 );
  glEnd();
}

void drawCube()
{
  glColor3f(.9,.9,.9);
  drawCubeFill();
  glColor3f(0,0,0);
  drawCubeWire();
}
*/
