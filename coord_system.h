#ifndef _COORD_SYSTEM_H
#define _COORD_SYSTEM_H

#include <cstdint>
#include <random>
#include <utility>

#include "agl.h"
#include "types.h"

/*
 * Coordinate System Generator.
 * Namespace which provides high level functions to generate random
 * coordinates to place various elements in the game. It provides both general
 * random coords and random coords specific to one of the 4 quadrants. It
 * provides also coords for a "3D Space", in which I mean with Y-coord != 0
 */

namespace coordinateGenerator {
// -- 2D PLANE: Y-coord = 0 -- //

// random coordinate for a specific quadrant
std::pair<float, float> genCoord2D(uint8_t quadrant);

// random coordinate generator for 2D plane
std::pair<float, float> randomCoord2D();

// coordinates for each quadrant in a 2D plane
// all are abstractions on top of genCoord2D
std::pair<float, float> firstQuadCoord2D();
std::pair<float, float> secondQuadCoord2D();
std::pair<float, float> thirdQuadCoord2D();
std::pair<float, float> fourthQuadCoord2D();

// -- 3D CUBE: Y-coord > 0 -- //

// random coordinate for a specific quadrant
agl::Point3 genCoord3D(uint8_t quadrant);

// random coordinate generator for 3D plane
agl::Point3 randomCoord3D();

// coordinates for each quadrant in 3D cube space, with y > 0
agl::Point3 firstQuadCoord3D();
agl::Point3 secondQuadCoord3D();
agl::Point3 thirdQuadCoord3D();
agl::Point3 fourthQuadCoord3D();

} // namespace coordinateGenerator

#endif // _COORD_SYSTEM_H