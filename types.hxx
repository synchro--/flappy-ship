#ifndef TYPES_HXX
#define TYPES_HXX

enum State { MENU, GAME, SETTINGS, RANKING };
enum Key {
    W,A,S,D,UP,DOWN,LEFT,RIGHT,ESC,RETURN
  };

// Default colors
const Color WHITE  = {1.0f, 1.0f, 1.0f, 1.0f};
const Color BLACK  = { .0f,  .0f,  .0f, 1.0f};
const Color RED    = {.77f, .01f, .2f};
const Color GREEN  = {.0f, .62f, .42f};
const Color YELLOW = {1.0f, .83f, .0f};

#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5

#endif
