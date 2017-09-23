// BUFFER FILE CONTENENTE VECCHIO CODICE CHE MAGARI PUÒ SERVIRE
// IN FUTURO PER QUALSIVOGLIA MOTIVO

// vecchio codice ora commentato
// disegna un cubo rasterizzando quads
void drawCubeFill() {
  const float S = 100;

  glBegin(GL_QUADS);

  glNormal3f(0, 0, +1);
  glVertex3f(+S, +S, +S);
  glVertex3f(-S, +S, +S);
  glVertex3f(-S, -S, +S);
  glVertex3f(+S, -S, +S);

  glNormal3f(0, 0, -1);
  glVertex3f(+S, -S, -S);
  glVertex3f(-S, -S, -S);
  glVertex3f(-S, +S, -S);
  glVertex3f(+S, +S, -S);

  glNormal3f(0, +1, 0);
  glVertex3f(+S, +S, +S);
  glVertex3f(-S, +S, +S);
  glVertex3f(-S, +S, -S);
  glVertex3f(+S, +S, -S);

  glNormal3f(0, -1, 0);
  glVertex3f(+S, -S, -S);
  glVertex3f(-S, -S, -S);
  glVertex3f(-S, -S, +S);
  glVertex3f(+S, -S, +S);

  glNormal3f(+1, 0, 0);
  glVertex3f(+S, +S, +S);
  glVertex3f(+S, -S, +S);
  glVertex3f(+S, -S, -S);
  glVertex3f(+S, +S, -S);

  glNormal3f(-1, 0, 0);
  glVertex3f(-S, +S, -S);
  glVertex3f(-S, -S, -S);
  glVertex3f(-S, -S, +S);
  glVertex3f(-S, +S, +S);

  glEnd();
}

// disegna un cubo in wireframe
void drawCubeWire() {
  glBegin(GL_LINE_LOOP); // faccia z=+1
  glVertex3f(+1, +1, +1);
  glVertex3f(-1, +1, +1);
  glVertex3f(-1, -1, +1);
  glVertex3f(+1, -1, +1);
  glEnd();

  glBegin(GL_LINE_LOOP); // faccia z=-1
  glVertex3f(+1, -1, -1);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, +1, -1);
  glVertex3f(+1, +1, -1);
  glEnd();

  glBegin(GL_LINES); // 4 segmenti da -z a +z
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, -1, +1);

  glVertex3f(+1, -1, -1);
  glVertex3f(+1, -1, +1);

  glVertex3f(+1, +1, -1);
  glVertex3f(+1, +1, +1);

  glVertex3f(-1, +1, -1);
  glVertex3f(-1, +1, +1);
  glEnd();
}

void drawCube() {
  glColor3f(.9, .9, .9);
  drawCubeFill();
  glColor3f(0, 0, 0);
  drawCubeWire();
}

// Draws a torus of inner radius r and outer radius R.
void Env::drawTorus(float r, float R) {
  const static int NUM_C = 50;
  // number of vertex that approximates the circular ring shape
  const static int NUM_VERTEX_APPROX = 15;
  // length of the perimeter of the ring
  const static double RING_PERIMETER = 2.0 * M_PI;
  double s, t, x, y, z;
  double cos_phi, sin_phi, cos_teta, sin_teta;

  for (size_t i = 0; i < NUM_C; ++i) {
    glBegin(GL_QUAD_STRIP);

    for (size_t j = 0; j <= NUM_VERTEX_APPROX; ++j) {
      for (int k = 1; k >= 0; --k) {
        s = (i + k) % NUM_C + 0.5;
        t = j % NUM_VERTEX_APPROX;

        cos_phi = cos(s * RING_PERIMETER / NUM_C);
        sin_phi = sin(s * RING_PERIMETER / NUM_C);

        cos_teta = cos(t * RING_PERIMETER / NUM_VERTEX_APPROX);
        sin_teta = sin(t * RING_PERIMETER / NUM_VERTEX_APPROX);

        x = (R + r * cos_phi) * cos_teta;
        y = (R + r * cos_phi) * sin_teta;
        z = r * sin_teta;

        /*
        double
          n_x = cos_psi * cos_phi,
          n_y = sin_psi * cos_phi,
          n_z = sin_phi;

        glNormal3d(n_x, n_y, n_z);*/
        glNormal3d(x, y, z);
        glVertex3d(2 * x, 2 * y, 2 * z);
      }
    }

    glEnd();
  }
}

//old do_step computation 
----
  // computiamo l'evolversi della macchina
  float speed_xm, speed_ym, speed_zm; // velocita' in spazio macchina

  // da vel frame mondo a vel frame macchina
//  float cosf = cos(m_facing * M_PI / 180.0);
//  float sinf = sin(m_facing * M_PI / 180.0);
  float cosf = cos(m_angle * M_PI / 180.0);
  float sinf = sin(m_angle * M_PI / 180.0);
  speed_xm = +cosf * m_speedX - sinf * m_speedZ;
  //speed_ym = m_speedY;
  speed_zm = +sinf * m_speedX + cosf * m_speedZ;

  bool left = get_state(Motion::STEER_L);
  bool right = get_state(Motion::STEER_R);

  if (left ^ right) {
    int sign = left ? 1 : -1;

    m_steering += sign * m_steer_speed;
    m_steering *= m_steer_return;
  }

  bool throttle = get_state(Motion::THROTTLE);
  bool brake = get_state(Motion::BRAKE);

  if (throttle ^ brake) {
     lg::i(__func__, "lolo");
    int sign = throttle ? 1 : -1;

    m_speedZ += sign * m_max_acceleration;

    // Spaceships don't fly backwards
    //m_speedZ = (m_speedZ > 0.05) ? 0 : m_speedZ;
  }

  speed_xm *= m_frictionX;
  //speed_ym *= m_frictionY;
  speed_zm *= m_frictionZ;

  // l'orientamento della macchina segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  // ANGLE O FACING QUI??
  m_angle = m_angle - (speed_zm * m_grip) * m_steering;

  /* // rotazione mozzo ruote (a seconda della velocita' sulla z)
   float da; // delta angolo
   da = (360.0 * speed_zm) / (2.0 * M_PI * raggioRuotaA);
   mozzoA += da;
   da = (360.0 * speed_zm) / (2.0 * M_PI * raggioRuotaP);
   mozzoP += da; */

  // ritorno a vel coord mondo
  m_speedX = +cosf * speed_xm + sinf * speed_zm;
  m_speedY = speed_ym;
  m_speedZ = -sinf * speed_xm + cosf * speed_zm;

  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  m_px += m_speedX;
 // m_py += m_speedY;
  m_pz += m_speedZ;
}


        /* DOVREBBE ESSERE INUTILE
        else {
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {

              m_win.
              glViewport(0, 0, scrW, scrH);
              rendering(win, car);
              // redraw(); // richiedi ridisegno
              break;
            }
            }
          }
        } //ELSE */

// HANDLING MOUSE WHEEL DISTANCE 

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


  //--------------- JOYSTICK HANDLING ------------------------ // 
/*
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