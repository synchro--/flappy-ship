#include <cmath>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "agl.h"
#include "elements.h"
#include "game.h"
#include "types.h"

using namespace game;

int main(int argc, char *argv[]) {
  SDL_Window *win;
  SDL_GLEnv mainEnv;
  Uint32 windowID;
  SDL_Joystick *joystick;
  static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

  Car car; // our SpaceShip!
  Game gs; // current state of the game!

  // temporary
  int fpsNow = car.fpsNow;
  int scrW = car.scrW;
  int scrH = car.scrH;
  float m_view_alpha = car.m_view_alpha;
  float m_view_beta = car.m_view_beta;
  float m_eye_dist = car.m_eye_dist;
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

  // Create our opengl Env and attach it to our window
  mainEnv = SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali
                          // prima di usarle

  // glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_POLYGON_OFFSET_FILL);

  // Sposta i frammenti generati dalla rasterizzazione dei poligoni
  // indietro di 1
  glPolygonOffset(1, 1);

  // main event loop
  // in un metodo della classe Env
  // verrà utilizzato poi da game

  bool quit = false;
  while (!quit) {

    SDL_Event e;

    // check and process events
    if (SDL_PollEvent(&e)) {
      switch (e.type) {

      case SDL_KEYUP:
      case SDL_KEYDOWN: {

        // choose the proper callback handler according if key is pressed or
        // released
        auto handler =
            (e.type == SDL_KEYUP) ? m_key_up_handler : m_key_down_handler;

        switch (e.key.keysym.sym) {
        case SDLK_w:
          handler(Key::W);
          break;

        case SDLK_a:
          handler(Key::A);
          break;

        case SDLK_s:
          handler(Key::S);
          break;

        case SDLK_d:
          handler(Key::D);
          break;

        case SDLK_UP:
          handler(Key::UP);
          break;

        case SDLK_LEFT:
          handler(Key::LEFT);
          break;

        case SDLK_DOWN:
          handler(Key::DOWN);
          break;

        case SDLK_RIGHT:
          handler(Key::RIGHT);
          break;

        case SDLK_ESCAPE:
          handler(Key::ESC);
          break;

        case SDLK_RETURN:
          handler(Key::RETURN);
          break;

        case SDLK_F1:
          handler(Key::F1);
          break;

        case SDLK_F2:
          handler(Key::F2);
          break;

        case SDLK_F3:
          handler(Key::F3);
          break;

        case SDLK_F4:
          handler(Key::F4);
          break;

        case SDLK_F5:
          handler(Key::F5);
          break;

        default:
          break;
        }

        break;
      }

      case SDL_QUIT:
        quit = true;
        break;

      case SDL_WINDOWEVENT:
        // let's redraw the window
        if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
          m_window_event_handler();

        // da modificare
        else {
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {

              m_screenW = e.window.data1;
              m_screenH = e.window.data2;
              glViewport(0, 0, scrW, scrH);
              rendering(win, car);
              // redraw(); // richiedi ridisegno
              break;
            }
            }
          }
        }
        break;
      }

      /*
      TODO: handle joystick and mouse motion

     case SDL_MOUSEMOTION:
       handler = m_mouse_event_handler();
       if (e.motion.state & SDL_BUTTON(1) & m_camera_type == CAMERA_MOUSE) {
         view_alpha += e.motion.xrel;
         view_beta += e.motion.yrel;
         // if (m_view_beta<-90) m_view_beta=-90;
         if (view_beta < +5)
           view_beta = +5; // per non andare sotto la macchina
         if (view_beta > +90)
           view_beta = +90;


       }
       break;

     case SDL_MOUSEWHEEL:
       if (e.wheel.y < 0) {
         // avvicino il punto di vista (zoom in)
         m_eye_dist = m_eye_dist * 0.9;
         m_eye_dist =
             m_eye_dist < 1 ? 1 : m_eye_dist; // eyedist can't less than 1
       };
       if (e.wheel.y > 0) {
         // allontano il punto di vista (zoom out)
         m_eye_dist = m_eye_dist / 0.9;
       };
       break;

     case SDL_JOYAXISMOTION: // Handle Joystick Motion
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

       //handle joystick buttons
     case SDL_JOYBUTTONDOWN:
       if (e.jbutton.button == 0) {
         car.controller.Joy(2, true);
         //	   printf("jbutton 0\n");
       }
       if (e.jbutton.button == 2) {
         car.controller.Joy(3, true);
         //	   printf("jbutton 2\n");
       }
       break;
     case SDL_JOYBUTTONUP:
       car.controller.Joy(2, false);
       car.controller.Joy(3, false);
       break;
     }
   }

   */

      else {
        // nessun evento: siamo IDLE

        // dentro env
        Uint32 time_now = m_env.getTicks();

        if (m_time_last_interval + FPS_SAMPLE < time_now) {
          car.fps =
              1000.0 * ((float)m_fps_now) / (time_now - m_time_last_interval);
          m_fps_now = 0;
          m_time_last_interval = time_now;
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
            quit = true;
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

    // Se ne occupa la distruzione della Window
    SDL_GL_DeleteContext(mainEnv);
    SDL_DestroyWindow(win);

    return (0);
  }

  /*
  what the main should look like, in the end

  #include "game.hh"
  #include "log.hh"

  #include <cstdlib>
  #include <iostream>


  int main(void) {
      lg::set_level(lg::Level::INFO);

      game::Game game();
      game.run();

      //praying god everything's run ok
      return EXIT_SUCCESS;
  }

  */
