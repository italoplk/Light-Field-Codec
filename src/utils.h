#ifndef __UTILS_H__
#define __UTILS_H__

#include "Point4D.h"
#include <vector>

template <typename T>
inline Point4D make_stride(T shape) {
    Point4D stride;
    stride.x = 1;
    stride.y = shape[0];
    stride.u = stride.y * shape[1];
    stride.v = stride.u * shape[2];
    return stride;
}

template <typename size_type>
auto make_shapes(const std::vector<size_type> &from_shape,
                 const std::vector<size_type> &base_shape) {
    std::vector<std::vector<size_type>> shapes;
    std::vector<size_type> shape;
    shape.resize(4);
    for (int v = 0; v < from_shape[3]; v += shape[3]) {
        shape[3] = std::min(base_shape[3], from_shape[3] - v);
        for (int u = 0; u < from_shape[2]; u += shape[2]) {
            shape[2] = std::min(base_shape[3], from_shape[2] - u);
            for (int y = 0; y < from_shape[1]; y += shape[1]) {
                shape[1] = std::min(base_shape[1], from_shape[1] - y);
                for (int x = 0; x < from_shape[0]; x += shape[0]) {
                    shape[0] = std::min(base_shape[0], from_shape[0] - x);
                    shapes.push_back(shape);
                }
            }
        }
    }
    return shapes;
}

inline int offset(int x, int y, int u, int v, Point4D &stride) {
    return x * stride.x + y * stride.y + u * stride.u + v * stride.v;
}



#endif // __UTILS_H__