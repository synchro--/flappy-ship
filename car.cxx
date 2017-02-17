// implementazione dei metodi definiti in car.h
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdio>
#include <vector> // la classe vector di STL

#include "point3.hxx"
#include "mesh.hxx"

#include "car.hxx"
#include "utils.hxx"

// Spaceship
Mesh carlinga((char *)"Envos.obj");
// var globale di tipo mesh
// Mesh carlinga((char *)"Ferrari_chassis.obj"); // chiama il costruttore
Mesh wheelBR1((char *)"Ferrari_wheel_back_R.obj");
Mesh pista((char *)"pista.obj");

extern bool useEnvmap; // var globale esterna: per usare l'evnrionment mapping
extern bool useHeadlight; // var globale esterna: per usare i fari
extern bool useShadow;    // var globale esterna: per generare l'ombra

// da invocare quando e' stato premuto/rilasciato il tasto numero "keycode"
void Controller::EatKey(int keycode, int *keymap, bool pressed_or_released) {
  for (int i = 0; i < NKEYS; i++) {
    if (keycode == keymap[i])
      key[i] = pressed_or_released;
  }
}

// da invocare quando e' stato premuto/rilasciato un jbutton
void Controller::Joy(int keymap, bool pressed_or_released) {
  key[keymap] = pressed_or_released;
}

// Funzione che prepara tutto per usare un env map
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

// void drawCube(); // questa e' definita altrove (quick hack)
void drawAxis(); // anche questa

void drawPista() {
  scope([](void) {
    glColor3f(0.4, 0.4, .8);
    glScalef(0.75, 1.0, 0.75);
    glTranslatef(0, 0.01, 0);
    // pista.RenderNxV();
    pista.RenderNxF();
  });
}

/*
// diesgna una ruota come due cubi intersecati a 45 gradi
void drawWheel(){
  glPushMatrix();
  glScalef(1, 1.0/sqrt(2.0),  1.0/sqrt(2.0));
  drawCube();
  glRotatef(45,  1,0,0);
  drawCube();
  glPopMatrix();
}
*/

void Controller::Init() {
  for (int i = 0; i < NKEYS; i++)
    key[i] = false;
}

void Car::Init() {
  // inizializzo lo stato della macchina
  px = pz = facing = 0; // posizione e orientamento
  py = 0.0;

  mozzoA = mozzoP = sterzo = 0; // stato
  vx = vy = vz = 0;             // velocita' attuale
  // inizializzo la struttura di controllo
  controller.Init();

  velSterzo = 3.4;         // A
                           // velSterzo = 2.4;         // A
  velRitornoSterzo = 0.93; // B, sterzo massimo = A*B / (1-B)

  accMax = 0.0011;

  // attriti: percentuale di velocita' che viene mantenuta
  // 1 = no attrito
  // <<1 = attrito grande
  attritoZ =
      0.991; // piccolo attrito sulla Z (nel senso di rotolamento delle ruote)
  attritoX = 0.9; // grande attrito sulla X (per non fare slittare la macchina)
  attritoY = 1.0; // attrito sulla y nullo

  // Nota: vel max = accMax*attritoZ / (1-attritoZ)

  raggioRuotaA = 0.25;
  raggioRuotaP = 0.35;

  grip = 0.45; // quanto il facing macchina si adegua velocemente allo sterzo
}

/*
//vecchio codice ora commentato
// disegna carlinga composta da 1 cubo traslato e scalato
static void drawCarlinga(){
  // disegna carlinga

  glColor3f(1,0,0);

  // sono nel frame CAR
  glPushMatrix();

  // vado al frame pezzo_A
  glScalef(0.25 , 0.14 , 1);
  drawCube();

  // torno al frame CAR
  glPopMatrix();

  // vado frame pezzo_B
  glPushMatrix();
  glTranslatef(0,-0.11,-0.95);
  glScalef(0.6, 0.05, 0.15);
  drawCube();
  glPopMatrix();

   // vado frame pezzo_C
  glPushMatrix();
  glTranslatef(0,-0.11,0);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();

  // vado frame pezzo_D
  glPushMatrix();
  glRotatef(-5,1,0,0);
  glTranslatef(0,+0.2,+0.95);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();
}
*/

// attiva una luce di openGL per simulare un faro
void Car::DrawHeadlight(float x, float y, float z, int lightN,
                        bool useHeadlight) const {
  int usedLight = GL_LIGHT1 + lightN;

  if (useHeadlight) {
    glEnable(usedLight);

    float col0[4] = {0.8, 0.8, 0.0, 1};
    glLightfv(usedLight, GL_DIFFUSE, col0);

    float col1[4] = {0.5, 0.5, 0.0, 1};
    glLightfv(usedLight, GL_AMBIENT, col1);

    float tmpPos[4] = {x, y, z, 1}; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_POSITION, tmpPos);

    float tmpDir[4] = {0, 0, -1, 0}; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir);

    glLightf(usedLight, GL_SPOT_CUTOFF, 30);
    glLightf(usedLight, GL_SPOT_EXPONENT, 5);

    glLightf(usedLight, GL_CONSTANT_ATTENUATION, 0);
    glLightf(usedLight, GL_LINEAR_ATTENUATION, 1);
  } else
    glDisable(usedLight);
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
  scope([&](void) {
    //vado al centro dell'astronave e la ruoto di un angolo definito dallo sterzo 
    //rispetto all'asse z
    int sign = -1;
    glTranslate(carlinga.Center());
    glRotatef(sign * sterzo, 0, 0, 1);
    glTranslate(-carlinga.Center());
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


void Car::Shadow() const {
    glColor3f(0.4, 0.4, 0.4); // colore fisso
    glTranslatef(0, 0.01, 0); // alzo l'ombra di un epsilon per evitare
                              // z-fighting con il pavimento
    glScalef(
        1.01, 0,
        1.01); // appiattisco sulla Y, ingrandisco dell'1% sulla Z e sulla X
    glDisable(GL_LIGHTING); // niente lighing per l'ombra
    RenderAllParts(false);  // disegno la macchina appiattita
    glEnable(GL_LIGHTING);
}