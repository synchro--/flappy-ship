#ifndef CAR_HXX

#define CAR_HXX

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdio>
#include <vector> // la classe vector di STL

#include "point3.hxx"

#include "mesh.hxx"
#include "utils.hxx"
#include "game.hxx"

class Controller {
public:
  enum { LEFT = 0, RIGHT = 1, ACC = 2, DEC = 3, NKEYS = 4 };
  bool key[NKEYS];
  void Init();
  void EatKey(int keycode, int *keymap, bool pressed_or_released);
  void Joy(int keymap, bool pressed_or_released);
  Controller() { Init(); } // costruttore
};

class Car : public Game {

  void RenderAllParts(bool usecolor) const;
  // disegna tutte le parti della macchina
  // invocato due volte: per la car e la sua ombra

public:
  // Metodi
  void Init();         // inizializza variabili
  void Render() const; // disegna a schermo
  void DoStep();       // computa un passo del motore fisico
  //utils from cars 
  void Shadow() const; 
  void setCamera();

  Car() { Init(); }    // costruttore

  Controller controller;

  // STATO DELLA MACCHINA
  // (DoStep fa evolvere queste variabili nel tempo)
  float px, py, pz, facing;     // posizione e orientamento
  float mozzoA, mozzoP, sterzo; // stato interno
  float vx, vy, vz;             // velocita' attuale

  // STATS DELLA MACCHINA
  // (di solito rimangono costanti)
  float velSterzo, velRitornoSterzo, accMax, attrito, raggioRuotaA,
      raggioRuotaP, grip, attritoX, attritoY, attritoZ; // attriti

private:
  void DrawHeadlight(float x, float y, float z, int lightN,
                     bool useHeadlight) const;
};

#endif