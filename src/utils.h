#ifndef __UTILS_H__
#define __UTILS_H__

#include "Typedef.h"
#include "Point4D.h"

#include <vector>
#include <opencv2/opencv.hpp>
#include <cmath>

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

template <typename size_type>
auto make_shapes(const std::vector<size_type> &from_shape, int levels) {
       
    std::vector<std::vector<size_type>> shapes;
    std::vector segment_shape = from_shape;
    std::vector remainder = from_shape;
    

    auto it_seg = segment_shape.begin();
    auto it_rem = remainder.begin();
    for (; it_seg < segment_shape.end(); it_seg++, it_rem++) {
        *it_seg = *it_seg >> levels;
        *it_rem = *it_rem - (*it_seg << levels);
    }
    for (int v = 0; v < (1 << levels); v++) {
        for (int u = 0; u < (1 << levels); u++) {
            for (int y = 0; y < (1 << levels); y++) {
                for (int x = 0; x < (1 << levels); x++) {
                    std::vector curr_seg = segment_shape;
                    if (x < remainder[0])
                        curr_seg[0]++;
                    if (y < remainder[1])
                        curr_seg[1]++;
                    if (u < remainder[2])
                        curr_seg[2]++;
                    if (v < remainder[3])
                        curr_seg[3]++;
                    shapes.push_back(curr_seg);
                }
            }
        }
    }
    return shapes;
}

inline int offset(int x, int y, int u, int v, const Point4D &stride) {
    return x * stride.x + y * stride.y + u * stride.u + v * stride.v;
}


template <typename T, typename size_type>
void segment_block(const T *from_block,
                              const std::vector<size_type> &from_shape,
                              T *into_block,
                              int levels) {
    auto *curr_seg = into_block;
    auto stride = make_stride(from_shape);
    int seg_index = 0;
    const auto shapes = make_shapes(from_shape, levels);
    auto shape = shapes[seg_index];
    for (int v = 0; v < from_shape[3]; v += shape[3]) {
        for (int u = 0; u < from_shape[2]; u += shape[2]) {
            for (int y = 0; y < from_shape[1]; y += shape[1]) {
                for (int x = 0; x < from_shape[0]; x += shape[0]) {
                    shape = shapes[seg_index++];
                    auto into_stride = make_stride(shape);
                    for (int dv = 0; dv < shape[3]; dv++)
                        for (int du = 0; du < shape[2]; du++)
                            for (int dy = 0; dy < shape[1]; dy++) {
                                auto from_offset = stride.x * x + stride.y * (y + dy) +
                                                   stride.u * (u + du) + stride.v * (v + dv);

                                std::copy(from_block + from_offset,
                                          from_block + from_offset + shape[0], curr_seg);
                                curr_seg += shape[0];
                            }
                }
            }
        }
    }
}
template <typename T, typename size_type>
void join_segments(const T *from_block,
                              const std::vector<size_type> &from_shape,
                              T *into_block,
                              int levels)
{
    auto *curr_seg = from_block;
    auto stride = make_stride(from_shape);
    int seg_index = 0;
    const auto shapes = make_shapes(from_shape, levels);
    auto shape = shapes[seg_index];
    for (int v = 0; v < from_shape[3]; v += shape[3]) {
        for (int u = 0; u < from_shape[2]; u += shape[2]) {
            for (int y = 0; y < from_shape[1]; y += shape[1]) {
                for (int x = 0; x < from_shape[0]; x += shape[0]) {
                    shape = shapes[seg_index++];
                    auto into_stride = make_stride(shape);
                    for (int dv = 0; dv < shape[3]; dv++)
                        for (int du = 0; du < shape[2]; du++)
                            for (int dy = 0; dy < shape[1]; dy++) {
                                auto into_offset = stride.x * x + stride.y * (y + dy) +
                                                   stride.u * (u + du) + stride.v * (v + dv);
                                std::copy(curr_seg, curr_seg + shape[0], into_block + into_offset);
                                curr_seg += shape[0];
                            }
                }
            }
        }
    }
}

inline void extend_borders(float *block, const Point4D &shape, const Point4D &stride);

void show_block(int channel, float *block, const Point4D &shape, const Point4D &stride, const char *window);

void progress_bar(double progress, int bar_length);

void flip_axis(float *block, unsigned to_flip, unsigned flat_size, Point4D shape, Point4D stride);

#endif // __UTILS_H__