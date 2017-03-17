#include <cmath>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "car.hxx"
#include "game.hxx"
#include "types.hxx"
#include "utils.hxx"

const int PHYS_SAMPLING_STEP =
    10; // numero di millisec che un passo di fisica simula
// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps

extern void drawPista();

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win, Car car) {

  int fpsNow = car.fpsNow;
  int scrW = car.scrW;
  int scrH = car.scrH;
  float fps = car.fps;

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
  car.setCamera();

  // drawAxis(); // disegna assi frame MONDO

  static float tmpcol[4] = {1, 1, 1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  glEnable(GL_LIGHTING);

  // settiamo matrice di modellazione
  // drawAxis(); // disegna assi frame OGGETTO
  // drawCubeWire();

  drawSky(car); // disegna il cielo come sfondo

  drawFloor(); // disegna il suolo
  drawPista(); // disegna la pista

  car.Render(); // disegna la macchina

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // disegnamo i fps (frame x sec) come una barra a sinistra.
  // (vuota = 0 fps, piena = 100 fps)
  SetCoordToPixel(car);

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

int main(int argc, char *argv[]) {
  SDL_Window *win;
  SDL_GLContext mainContext;
  Uint32 windowID;
  SDL_Joystick *joystick;
  static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

  Car car; // our SpaceShip!
  Game gs; // current state of the game!

  // temporary
  int fpsNow = car.fpsNow;
  int scrW = car.scrW;
  int scrH = car.scrH;
  float viewAlpha = car.viewAlpha;
  float viewBeta = car.viewBeta;
  float eyeDist = car.eyeDist;
  int nstep = car.nstep;
  bool useWireframe = car.useWireframe;
  bool useEnvmap = car.useEnvmap;
  bool useHeadlight = car.useHeadlight;
  bool useShadow = car.useShadow;
  int cameraType = car.cameraType;
  Uint32 timeLastInterval = car.timeLastInterval;

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
  if (!LoadTexture(2, (char *)"space1.jpg"))
    return -1;

  bool done = 0;
  while (!done) {

    SDL_Event e;

    // check and process events
    if (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_c:
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
          rendering(win, car);
        else {
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
              scrW = e.window.data1;
              scrH = e.window.data2;
              glViewport(0, 0, scrW, scrH);
              rendering(win, car);
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
          rendering(win, car);
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
        car.fps = 1000.0 * ((float)fpsNow) / (timeNow - timeLastInterval);
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
        rendering(win, car);
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
