// implementazione dei metodi definiti in car.h
#include "car.h"

// Funzione che prepara tutto per usare un env map
// CONFRONTARE CON QUELLE CHE USO IO PER LE TEXTURE
void SetupEnvmapTexture() {
  // facciamo binding con la texture 1
  glBindTexture(GL_TEXTURE_2D, 1);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord
                              // texture S e T
  glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); // Env map
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glColor3f(1, 1, 1); // metto il colore neutro (viene moltiplicato col colore
                      // texture, componente per componente)
  glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio
                          // con la texture)
}

// DA CHIARIRE
// funzione che prepara tutto per creare le coordinate texture (s,t) da (x,y,z)
// Mappo l'intervallo [ minY , maxY ] nell'intervallo delle T [0..1]
//     e l'intervallo [ minZ , maxZ ] nell'intervallo delle S [0..1]
void SetupWheelTexture(Point3 min, Point3 max) {
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  // utilizzo le coordinate OGGETTO
  // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
  // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su
  // esso
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  float sz = 1.0 / (max.Z() - min.Z());
  float ty = 1.0 / (max.Y() - min.Y());
  float s[4] = {0, 0, sz, -min.Z() * sz};
  float t[4] = {0, ty, 0, -min.Y() * ty};
  glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
  glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
}

// DoStep: facciamo un passo di fisica (a delta_t costante)
//
// Indipendente dal rendering.
//
// ricordiamoci che possiamo LEGGERE ma mai SCRIVERE
// la struttura controller da DoStep
void Car::DoStep() {
  // computiamo l'evolversi della macchina
  float vxm, vym, vzm; // velocita' in spazio macchina

  // da vel frame mondo a vel frame macchina
  float cosf = cos(facing * M_PI / 180.0);
  float sinf = sin(facing * M_PI / 180.0);
  vxm = +cosf * vx - sinf * vz;
  vym = vy;
  vzm = +sinf * vx + cosf * vz;

  // gestione dello sterzo
  if (controller.key[Controller::LEFT])
    sterzo += velSterzo;
  if (controller.key[Controller::RIGHT])
    sterzo -= velSterzo;
  sterzo *= velRitornoSterzo; // ritorno a volante dritto

  if (controller.key[Controller::ACC])
    vzm -= accMax; // accelerazione in avanti
  if (controller.key[Controller::DEC])
    vzm += accMax; // accelerazione indietro

  // attirti (semplificando)
  vxm *= attritoX;
  vym *= attritoY;
  vzm *= attritoZ;

  // l'orientamento della macchina segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm * grip) * sterzo;

  // rotazione mozzo ruote (a seconda della velocita' sulla z)
  float da; // delta angolo
  da = (360.0 * vzm) / (2.0 * M_PI * raggioRuotaA);
  mozzoA += da;
  da = (360.0 * vzm) / (2.0 * M_PI * raggioRuotaP);
  mozzoP += da;

  // ritorno a vel coord mondo
  vx = +cosf * vxm + sinf * vzm;
  vy = vym;
  vz = -sinf * vxm + cosf * vzm;

  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  px += vx;
  py += vy;
  pz += vz;
}

void drawPista() {
  mat_scope([](void) {
    glColor3f(0.4, 0.4, .8);
    glScalef(0.75, 1.0, 0.75);
    glTranslatef(0, 0.01, 0);
    // pista.RenderNxV();
    pista.RenderNxF();
  });
}

// funzione che disegna tutti i pezzi della macchina
// (carlinga, + 4 route)
// (da invocarsi due volte: per la macchina, e per la sua ombra)
// (se usecolor e' falso, NON sovrascrive il colore corrente
//  e usa quello stabilito prima di chiamare la funzione)
void Car::RenderAllParts(bool usecolor) const {

  // drawCarlinga(); // disegna la carliga con pochi parallelepidedi

  // draw the spaceship using a mesh
  glPushMatrix();

  float sc_meshX, sc_meshY, sc_meshZ; // scaling factors
  sc_meshX = -0.005;
  sc_meshY = 0.005;
  sc_meshZ = -0.005;
  glScalef(sc_meshX, sc_meshY, sc_meshZ); // patch: riscaliamo la mesh

  if (!useEnvmap) {
    if (usecolor)
      glColor3f(1, 0, 0); // colore rosso, da usare con Lighting
  } else {
    if (usecolor)
      SetupEnvmapTexture();
  }
  if (usecolor) {
    glEnable(GL_LIGHTING);
  }

  // curva l'astronave a seconda dello sterzo
  mat_scope([&](void) {
    // vado al centro dell'astronave e la ruoto di un angolo definito dallo
    // sterzo
    // rispetto all'asse z
    int sign = -1;
    glTranslate(carlinga.Center());
    // questo lo faccio in rotate --> quindi in m_steering devo aggiungere il
    // segno
    glRotatef(sign * sterzo, 0, 0, 1);
    glTranslate(-carlinga.Center());
    // lo faccio già con la classe mesh
    carlinga.RenderNxV(); // render di carling using normal x vertex
  });

  glPopMatrix();
}

// disegna a schermo
void Car::Render() const {
  // sono nello spazio mondo

  // drawAxis(); // disegno assi spazio mondo
  glPushMatrix();
  // sono nello spazio MACCHINA

  glTranslatef(px, py, pz);
  glRotatef(facing, 0, 1, 0);

  // drawAxis(); // disegno assi spazio macchina

  DrawHeadlight(-0.3, 0, -1, 10, useHeadlight); // accendi faro sinistro
  DrawHeadlight(+0.3, 0, -1, 1, useHeadlight);  // accendi faro destro

  RenderAllParts(true);

  // ombra!
  if (useShadow) {
    Shadow();
  }

  glPopMatrix();
  glPopMatrix();
}
