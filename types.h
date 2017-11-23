#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>
#include <utility>

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>

// This header contains different data types used in the game.
// It's structured in different namespaces.

// definitions
#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5

// AGL TYPES
namespace agl {

// defining texture ID val, out of clarity
using TexID = GLuint;

// represents a RGBA color.
struct Color {
  float r, g, b, a;

  Color(float r = 0, float g = 0, float b = 0, float a = 1)
      : r(r), g(g), b(b), a(a) {}
};

// Default colors
const Color WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const Color BLACK = {.0f, .0f, .0f, 1.0f};
const Color RED = {.768f, .109f, .109f};
const Color GREEN = {.447f, .643f, .074f};
const Color YELLOW = {.913f, .643f, .074f};
const Color LIGHT_YELLOW = {245.0f, 246.0f, 206.0f};
const Color SHADOW = {.3f, .3f, .3f};

static const auto PHYS_SAMPLING_STEP = 10U; // millisec of a Physics sim step
static const auto FPS_SAMPLE = 10U;         // interval length
} // namespace agl

// GAME TYPES
namespace game {

static const auto RING_TIME = 7000U; // 8 secs
static const auto BONUS_TIME = 5000U;
static const auto FLAPPY_RING_TIME = 11000U; // 11 secs
static const auto FLAPPY_BONUS_TIME = 7000U;

using Entry = std::pair<std::string, double>;

struct Setting {
  bool &active; // reference to a setting, a bool value
  const char *name, *txt_on, *txt_off;

  // Setting(bool active, const char *name, const char *text_on, const char
  // *text_off)
  //   : active(active), name(name), txt_on(text_on), txt_off(text_off) {}
};

enum Settings { BLENDING, WIREFRAME, ENVMAP, FLAPPY3D, N_SETTINGS };
enum State { SPLASH, MENU, GAME, SETTINGS, END };
enum Key {
  W,
  A,
  S,
  D,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  ESC,
  RETURN,
  F1,
  F2,
  F3,
  F4,
  F5,
  N_KEYS
};
enum MouseEvent { MOTION, WHEEL };

} // namespace game

// LOGGING TYPES
namespace lg {
// logging levels
enum Level { INFO, ERROR, PANIC };
} // namespace lg

// SPACESHIP TYPES
namespace spaceship {
// Actions available for the Spaceship
// Note: can be expanded if the flying goes 3D, i.e. flying on the Y-axis too
enum Motion { THROTTLE, STEER_L, STEER_R, BRAKE, N_MOTION };

// Command data structure: a pair of <Enum Action, bool on/off>
// to be submitted to the Spaceship
using Command = std::pair<Motion, bool>;

// acceleration contants
static const auto VERY_FAST_ACC = 0.01;
static const auto FAST_ACC = 0.0045;
static const auto NORMAL_ACC = 0.0035;
static const auto FLIGHT_ACC = 0.078;

// Mesh dimension resizing constants
static const auto ENVOS_SCALE = 0.0059;
static const auto FALCON_SCALE = 0.0099;
static const auto SHUTTLE_SCALE = 0.013;
static const auto BOAT_SCALE = 0.17;

// Some mesh are built along a particular axis:
// Therefore a rotation is needed
static const auto ENVOS_ANGLE = 180;
static const auto SHUTTLE_ANGLE = 270;
static const auto BOAT_ANGLE = 270;

} // namespace spaceship

// ELEMENTS CONSTANTS
namespace elements {
static const auto FLOOR_SIZE = 120.0;
static const auto SKY_RADIUS = 120.0;
static const auto DOOR_SCALE = 0.7;
} // namespace elements

#endif //_TYPES_H_
