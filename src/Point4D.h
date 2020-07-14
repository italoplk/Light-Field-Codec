
#ifndef POINT4D_H
#define POINT4D_H

#include <iomanip>
#include <iostream>
#include <vector>
#include <zconf.h>

class Point4D {
public:
  uint x{8}, y{8}, u{8}, v{8};
  uint nSamples{0};

  Point4D() = default;

  Point4D(uint x, uint y, uint u, uint v);
  template <typename T>
  Point4D(const T *array): x(array[0]), y(array[1]), u(array[2]), v(array[3]) {updateNSamples();}

  void updateNSamples();

  uint getNSamples() const;

  std::vector<uint> to_vector() const {
    std::vector<uint> vec;
    vec.resize(4);
    vec[0] = x;
    vec[1] = y;
    vec[2] = u;
    vec[3] = v;
    return vec;
  }

  friend std::ostream &operator<<(std::ostream &os, Point4D const &point);

  friend bool operator!=(const Point4D &p1, const Point4D &p2);

  friend bool operator==(const Point4D &p1, const Point4D &p2);

  friend Point4D operator>>(const Point4D &p, const int val) {
    Point4D q;
    q.x = p.x >> val;
    q.y = p.y >> val;
    q.u = p.u >> val;
    q.v = p.v >> val;
    return q;
  }

  uint &operator[](size_t dimension) {
    switch (dimension) {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return u;
    case 3:
      return v;
    }
    throw;
  }

  friend bool operator<(const Point4D& p, const Point4D& q) {
    return p.x < q.x && p.y < q.y && p.u < q.u && p.v < q.v;
  }
};


struct Point4DHasher
{
  std::size_t operator () (const Point4D &p) const 
  {
    return p.x ^ ((p.y << 1) ^ ((p.u << 2) ^ (p.v << 3)));
  }
};

#endif // POINT4D_H
