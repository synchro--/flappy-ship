#include "agl.h"
#include <SDL2/SDL_ttf.h>

namespace agl {

/*
 * Main Render Loop:
 * ----------------
 * Renders the splash screen and then goes into an infinite loop processing
 * events as they arise.
 * - it dispatch the keys to the proper handler callbacks
 * - calls the action handler to update the game status
 * - finally, calls the rendering handler lambda: m_render_handler()
 */

void Env::renderLoop() {
  // main event loop

  m_render_handler();

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
        } // switch(key)

        break;
      } // SDL_KEYDOWN

      case SDL_QUIT: {
        quit = true;
        break;
      }

      case SDL_WINDOWEVENT: {
        // let's redraw the window
        if (e.window.event == SDL_WINDOWEVENT_EXPOSED ||
            SDL_WINDOWEVENT_SIZE_CHANGED) {
          m_window_event_handler();
        }
        break;
      }

        // ---- MOUSE EVENTS --- //

      case SDL_MOUSEMOTION: {
        auto handler = m_mouse_event_handler;

        if (e.motion.state & SDL_BUTTON(1)) {
          handler(MouseEvent::MOTION, e.motion.xrel, e.motion.yrel);
        }
      } break;

      case SDL_MOUSEWHEEL: {
        auto handler = m_mouse_event_handler;
        handler(MouseEvent::WHEEL, e.wheel.y, -1.0);
      } break;

      default:
        break;

      } // switch(e.type)

    } // if(SDL_PollEvent)

    m_action_handler();

    // Render once each cycle
    render();

  } // while loop

} // function mainLoop

} // namespace agl