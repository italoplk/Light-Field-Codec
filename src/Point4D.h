
#ifndef POINT4D_H
#define POINT4D_H

#include <iomanip>
#include <iostream>
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

  friend std::ostream &operator<<(std::ostream &os, Point4D const &point);

  friend bool operator!=(const Point4D &p1, const Point4D &p2);

  friend bool operator==(const Point4D &p1, const Point4D &p2);

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
};

#endif // POINT4D_H
