#include "agl.hxx"

/*
* The Point3 class defines all the operations regarding a 3D-Point,
* 3D-vector and 3D-vertex.
* Note that a 3D-vector (Vec3) is an alias for an actual Point3
*/

namespace agl {

// Construcor 1:
Point3::Point3(float x, float y, float z) : x(x), y(y), z(z) {}

// empty constructor:
Point3::Point3() : x(0), y(0), z(0) {}

// restituisce il modulo
float Point3::modulo() const { return sqrt(x * x + y * y + z * z); }

// restituisce la versione di se stesso normalizzata
Point3 Point3::normalize() const { return (*this) / modulo(); }

// operatore "/" binario: divisione per uno scalare (un float)
Point3 Point3::operator/(float f) const { return Point3(x / f, y / f, z / f); }

// operatore "-" unario: inversione del verso del vettore
Point3 Point3::operator-() const { return Point3(-x, -y, -z); }

// operatore "-" binario: differenza fra punti
Point3 Point3::operator-(const Point3 &other) const {
  return Point3(x - other.x, y - other.y, z - other.z);
}

// somma fra vettori
Point3 Point3::operator+(const Point3 &other) const {
  return Point3(x + other.x, y + other.y, z + other.z);
}

// ridefinisco l'operatore binario "%" per fare il CROSS PRODUCT
Point3 Point3::operator%(const Point3 &other) const {
  return Point3(y * other.z - z * other.y, -(x * other.z - z * other.x),
                x * other.y - y * other.x);
}

Normal3::Normal3(float x, float y, float z) : Vec3(x, y, z) {}
Normal3::Normal3(const Vec3 &vec3) : Vec3(vec3) {}

// send the vector as a normal
void Normal3::render() const {
  float coords[]{x, y, z};

  glNormal3fv(coords);
}

// Vertex constructor, initialize both its "views"
Vertex::Vertex(const Vec3 &v) : point(v), normal(0, 0, 0) {}

// if send_normal true, renders also the normal, otherwise only vertices
void Vertex::render(bool send_normal) const {
  if (send_normal) {
    normal.render();
  }

  float coords[]{point.x, point.y, point.z};
  glVertex3fv(coords);
}
}
