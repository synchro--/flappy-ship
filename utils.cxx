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