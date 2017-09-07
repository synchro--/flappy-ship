#ifndef TYPES_HXX
#define TYPES_HXX

namespace types {

enum State { MENU, GAME, SETTINGS, RANKING };
enum Key {
    W,A,S,D,UP,DOWN,LEFT,RIGHT,ESC,RETURN
  };

//logging levels
enum Level {
    INFO, ERROR, PANIC;
};


// Default colors
const Color WHITE  = {1.0f, 1.0f, 1.0f, 1.0f};
const Color BLACK  = { .0f,  .0f,  .0f, 1.0f};
const Color RED    = {.768f, .109f, .109f};
const Color GREEN  = {.447f, .643f, .074f};
const Color YELLOW = {.913f, .643f, .074f};



#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5

}

#endif
