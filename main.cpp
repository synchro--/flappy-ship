#include <cmath>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "car.h"

#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5

float viewAlpha = 20, viewBeta = 40; // angoli che definiscono la vista
float eyeDist = 5.0;                 // distanza dell'occhio dall'origine
int scrH = 750, scrW = 750;          // altezza e larghezza viewport (in pixels)
bool useWireframe = false;
bool useEnvmap = true;
bool useHeadlight = false;
bool useShadow = true;
int cameraType = 0;

Car car;       // la nostra macchina
int nstep = 0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP =
    10; // numero di millisec che un passo di fisica simula

// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps
float fps = 0;                // valore di fps dell'intervallo precedente
int fpsNow =
    0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval = 0; // quando e' cominciato l'ultimo intervallo

extern void drawPista();

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schemo
void SetCoordToPixel() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1, -1, 0);
  glScalef(2.0 / scrW, 2.0 / scrH, 1);
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

// setto la posizione della camera
void setCamera() {

  double px = car.px;
  double py = car.py;
  double pz = car.pz;
  double angle = car.facing;
  double cosf = cos(angle * M_PI / 180.0);
  double sinf = sin(angle * M_PI / 180.0);
  double camd, camh, ex, ey, ez, cx, cy, cz;
  double cosff, sinff;

  // controllo la posizione della camera a seconda dell'opzione selezionata
  switch (cameraType) {
  case CAMERA_BACK_CAR:
    camd = 2.5;
    camh = 1.0;
    ex = px + camd * sinf;
    ey = py + camh;
    ez = pz + camd * cosf;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_TOP_FIXED:
    camd = 0.5;
    camh = 0.55;
    angle = car.facing + 40.0;
    cosff = cos(angle * M_PI / 180.0);
    sinff = sin(angle * M_PI / 180.0);
    ex = px + camd * sinff;
    ey = py + camh;
    ez = pz + camd * cosff;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_TOP_CAR:
    camd = 2.5;
    camh = 1.0;
    ex = px + camd * sinf;
    ey = py + camh;
    ez = pz + camd * cosf;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey + 5, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_PILOT:
    camd = 0.2;
    camh = 0.55;
    ex = px + camd * sinf;
    ey = py + camh;
    ez = pz + camd * cosf;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_MOUSE:
    glTranslatef(0, 0, -eyeDist);
    glRotatef(viewBeta, 1, 0, 0);
    glRotatef(viewAlpha, 0, 1, 0);
    /*
    printf("%f %f %f\n",viewAlpha,viewBeta,eyeDist);
                    ex=eyeDist*cos(viewAlpha)*sin(viewBeta);
                    ey=eyeDist*sin(viewAlpha)*sin(viewBeta);
                    ez=eyeDist*cos(viewBeta);
                    cx = px - camd*sinf;
                    cy = py + camh;
                    cz = pz - camd*cosf;
                    gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
    */
    break;
  }
}

void drawSky() {
  int H = 100;

  if (useWireframe) {
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

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win) {

  // un frame in piu'!!!
  fpsNow++;

  glLineWidth(3); // linee larghe

  // settiamo il viewport
  glViewport(0, 0, scrW, scrH);

  // colore sfondo = bianco
  glClearColor(1, 1, 1, 1);

  // settiamo la matrice di proiezione
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70,                   // fovy,
                 ((float)scrW) / scrH, // aspect Y/X,
                 0.2, // distanza del NEAR CLIPPING PLANE in coordinate vista
                 1000 // distanza del FAR CLIPPING PLANE in coordinate vista
                 );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // riempe tutto lo screen buffer di pixel color sfondo
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // drawAxis(); // disegna assi frame VISTA

  // setto la posizione luce
  float tmpv[4] = {0, 1, 2, 0}; // ultima comp=0 => luce direzionale
  glLightfv(GL_LIGHT0, GL_POSITION, tmpv);

  // settiamo matrice di vista
  // glTranslatef(0,0,-eyeDist);
  // glRotatef(viewBeta,  1,0,0);
  // glRotatef(viewAlpha, 0,1,0);
  setCamera();

  // drawAxis(); // disegna assi frame MONDO

  static float tmpcol[4] = {1, 1, 1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  glEnable(GL_LIGHTING);

  // settiamo matrice di modellazione
  // drawAxis(); // disegna assi frame OGGETTO
  // drawCubeWire();

  drawSky(); // disegna il cielo come sfondo

  drawFloor(); // disegna il suolo
  drawPista(); // disegna la pista

  car.Render(); // disegna la macchina

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // disegnamo i fps (frame x sec) come una barra a sinistra.
  // (vuota = 0 fps, piena = 100 fps)
  SetCoordToPixel();

  glBegin(GL_QUADS);
  float y = scrH * fps / 100;
  float ramp = fps / 100;
  glColor3f(1 - ramp, 0, ramp);
  glVertex2d(10, 0);
  glVertex2d(10, y);
  glVertex2d(0, y);
  glVertex2d(0, 0);
  glEnd();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  glFinish();
  // ho finito: buffer di lavoro diventa visibile
  SDL_GL_SwapWindow(win);
}

void redraw() {
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type = SDL_WINDOWEVENT;
  e.window.event = SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}

int main(int argc, char *argv[]) {
  SDL_Window *win;
  SDL_GLContext mainContext;
  Uint32 windowID;
  SDL_Joystick *joystick;
  static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

  // inizializzazione di SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // facciamo una finestra di scrW x scrH pixels
  win = SDL_CreateWindow(argv[0], 0, 0, scrW, scrH,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  // Create our opengl context and attach it to our window
  mainContext = SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali
                          // prima di usarle
  // glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_POLYGON_OFFSET_FILL); // caro openGL sposta i
                                    // frammenti generati dalla
                                    // rasterizzazione poligoni
  glPolygonOffset(1, 1);            // indietro di 1

  if (!LoadTexture(0, (char *)"logo.jpg"))
    return 0;
  if (!LoadTexture(1, (char *)"envmap_flipped.jpg"))
    return 0;
  if (!LoadTexture(2, (char *)"sky_ok.jpg"))
    return -1;

  bool done = 0;
  while (!done) {

    SDL_Event e;

    // check and process events
    if (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          done = 1;
          break;
        }
        car.controller.EatKey(e.key.keysym.sym, keymap, true);
        
        /*switch(e.key.keysym.sym) {
          case SDLK_F1: 
                    cameraType = (cameraType + 1) % CAMERA_TYPE_MAX;
          case SDLK_F2: 
                    useWireframe = !useWireframe;

          case SDLK_F3: 
                    useEnvmap = !useEnvmap;

          case SDLK_F4: 
                    useHeadlight = !useHeadlight;

          case SDLK_F5: 
                    useShadow = !useShadow;
                    break;
        } */
        
        if (e.key.keysym.sym == SDLK_F1)
          cameraType = (cameraType + 1) % CAMERA_TYPE_MAX;
        if (e.key.keysym.sym == SDLK_F2)
          useWireframe = !useWireframe;
        if (e.key.keysym.sym == SDLK_F3)
          useEnvmap = !useEnvmap;
        if (e.key.keysym.sym == SDLK_F4)
          useHeadlight = !useHeadlight;
        if (e.key.keysym.sym == SDLK_F5)
          useShadow = !useShadow;
        break;
      case SDL_KEYUP:
        car.controller.EatKey(e.key.keysym.sym, keymap, false);
        break;
      case SDL_QUIT:
        done = 1;
        break;
      case SDL_WINDOWEVENT:
        // let's redraw the window
        if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
          rendering(win);
        else {
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
              scrW = e.window.data1;
              scrH = e.window.data2;
              glViewport(0, 0, scrW, scrH);
              rendering(win);
              // redraw(); // richiedi ridisegno
              break;
            }
            }
          }
        }
        break;

      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) & cameraType == CAMERA_MOUSE) {
          viewAlpha += e.motion.xrel;
          viewBeta += e.motion.yrel;
          // if (viewBeta<-90) viewBeta=-90;
          if (viewBeta < +5)
            viewBeta = +5; // per non andare sotto la macchina
          if (viewBeta > +90)
            viewBeta = +90;
          // redraw(); // richiedi un ridisegno (non c'e' bisongo: si ridisegna
          // gia'
          // al ritmo delle computazioni fisiche)
        }
        break;

      case SDL_MOUSEWHEEL:
        if (e.wheel.y < 0) {
          // avvicino il punto di vista (zoom in)
          eyeDist = eyeDist * 0.9;
          eyeDist = eyeDist < 1 ? 1 : eyeDist; // eyedist can't less than 1
        };
        if (e.wheel.y > 0) {
          // allontano il punto di vista (zoom out)
          eyeDist = eyeDist / 0.9;
        };
        break;

      case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
        if (e.jaxis.axis == 0) {
          if (e.jaxis.value < -3200) {
            car.controller.Joy(0, true);
            car.controller.Joy(1, false);
            //	      printf("%d <-3200 \n",e.jaxis.value);
          }
          if (e.jaxis.value > 3200) {
            car.controller.Joy(0, false);
            car.controller.Joy(1, true);
            //	      printf("%d >3200 \n",e.jaxis.value);
          }
          if (e.jaxis.value >= -3200 && e.jaxis.value <= 3200) {
            car.controller.Joy(0, false);
            car.controller.Joy(1, false);
            //	      printf("%d in [-3200,3200] \n",e.jaxis.value);
          }
          rendering(win);
          // redraw();
        }
        break;
      case SDL_JOYBUTTONDOWN: /* Handle Joystick Button Presses */
        if (e.jbutton.button == 0) {
          car.controller.Joy(2, true);
          //	   printf("jbutton 0\n");
        }
        if (e.jbutton.button == 2) {
          car.controller.Joy(3, true);
          //	   printf("jbutton 2\n");
        }
        break;
      case SDL_JOYBUTTONUP: /* Handle Joystick Button Presses */
        car.controller.Joy(2, false);
        car.controller.Joy(3, false);
        break;
      }
    } else {
      // nessun evento: siamo IDLE

      Uint32 timeNow = SDL_GetTicks(); // che ore sono?

      if (timeLastInterval + fpsSampling < timeNow) {
        fps = 1000.0 * ((float)fpsNow) / (timeNow - timeLastInterval);
        fpsNow = 0;
        timeLastInterval = timeNow;
      }

      bool doneSomething = false;
      int guardia = 0; // sicurezza da loop infinito

      // finche' il tempo simulato e' rimasto indietro rispetto
      // al tempo reale...
      while (nstep * PHYS_SAMPLING_STEP < timeNow) {
        car.DoStep();
        nstep++;
        doneSomething = true;
        timeNow = SDL_GetTicks();
        if (guardia++ > 1000) {
          done = true;
          break;
        } // siamo troppo lenti!
      }

      if (doneSomething)
        rendering(win);
      // redraw();
      else {
        // tempo libero!!!
      }
    }
  }
  SDL_GL_DeleteContext(mainContext);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return (0);
}
