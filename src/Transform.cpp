
#include "Transform.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include "utils.h"

std::map<size_t, float *> Transform::_DCT_II_CACHE;
std::map<size_t, float *> Transform::_DST_II_CACHE;
std::map<size_t, float *> Transform::_DST_VII_CACHE;

typedef void (*_tx_t)(const float *, float *, size_t, size_t);

void flip_axis(float *block, unsigned to_flip, unsigned flat_size, Point4D shape, Point4D stride) {
    float _block[flat_size];
    for (int v = 0; v < shape.v; v++) {
        for (int u = 0; u < shape.u; u++) {
            for (int y = 0; y < shape.y; y++) {
                for (int x = 0; x < shape.x; x++) {
                    auto dx = to_flip & Transform::AXIS_X ? shape.x - 1 - 2 * x : 0;
                    auto dy = to_flip & Transform::AXIS_Y ? shape.y - 1 - 2 * y : 0;
                    auto du = to_flip & Transform::AXIS_U ? shape.u - 1 - 2 * u : 0;
                    auto dv = to_flip & Transform::AXIS_V ? shape.v - 1 - 2 * v : 0;
                    auto f_offset = offset(x, y, u, v, stride);
                    auto r_offset = offset(x + dx, y + dy, u + du, v + dv, stride);
                    _block[r_offset] = block[f_offset];
                }
            }
        }
    }
    for (int v = 0; v < shape.v; v++) {
        for (int u = 0; u < shape.u; u++) {
            for (int y = 0; y < shape.y; y++) {
                for (int x = 0; x < shape.x; x++) {
                    auto index = offset(x, y, u, v, stride);
                    block[index] = _block[index];
                }
            }
        }
    }
}

float *Transform::_get_coefficients(Transform::TransformType type, const size_t size) {
    float *coeff = nullptr;
    switch (type) {
        case DST_II: try { coeff = _DST_II_CACHE.at(size);
            } catch (...) {
                coeff = _DST_II(size);
                _DST_II_CACHE[size] = coeff;
            }
            break;
        case DST_VII: try { coeff = _DST_VII_CACHE.at(size);
            } catch (...) {
                coeff = _DST_VII(size);
                _DST_VII_CACHE[size] = coeff;
            }
            break;
        case BESTMATCH:
        case DCT:
        default: try { coeff = _DCT_II_CACHE.at(size);
            } catch (...) {
                coeff = _DCT_II(size);
                _DCT_II_CACHE[size] = coeff;
            }
    }
    return coeff;
}

void Transform::_forward_1(Transform::TransformType type,
                           const float *in,
                           float *out,
                           const size_t offset,
                           const size_t size) {
    float *coeff = _get_coefficients(type, size);
    auto pout = out;
    auto pcoeff = coeff;
    for (int k = 0; k < size; k++, pout += offset) {
        auto pin = in;
        *pout = 0;
        for (int n = 0; n < size; n++, pin += offset, pcoeff++)
            *pout += *pin * *pcoeff;
    }
}

void Transform::_inverse_1(Transform::TransformType type,
                           const float *in,
                           float *out,
                           const size_t offset,
                           const size_t size) {
    float *coeff = _get_coefficients(type, size);
    auto pout = out;
    for (int k = 0; k < size; k++, pout += offset) {
        auto pin = in;
        auto pcoeff = coeff + k;
        *pout = 0;
        for (int n = 0; n < size; n++, pin += offset, pcoeff += size)
            *pout += *pin * *pcoeff;
    }
}

Transform::Transform(Point4D &shape) {
    this->shape = shape;
    stride = make_stride(shape);
    flat_size = stride.v * shape.v;
    flat_p2 = std::pow(2, std::ceil(std::log2(flat_size)));
    partial_values = new float[flat_size];
    wh_partial_values = new float[flat_p2];
}

Transform::~Transform() {
    delete[] partial_values;
    delete[] wh_partial_values;
}

float *Transform::_DST_VII(size_t size) {
    auto *output = new float[size * size];
    auto *pout = output;
    for (int k = 0; k < size; k++) {
        double s = (double)2 / (sqrt(2 * size + 1));
        for (int n = 0; n < size; n++)
            *pout++ = s * sin((double)M_PI * (n + 1) * (2 * k + 1) / (2 * size + 1));
    }
    return output;
}
float *Transform::_DST_II(size_t size) {
    auto *output = new float[size * size];
    auto *pout = output;
    double s = sqrt(2.0L / (size + 1.0L));
    for (int k = 0; k < size; k++) {
        for (int n = 0; n < size; n++) {
            double theta = (M_PI * (k + 1.0L) * (n + 1.0L)) / (size + 1.0L);
            *pout++ = s * sin(theta);
        }
    }
    return output;
}

float *Transform::_DCT_II(size_t size) {
    float *output = new float[size * size];
    float *pout = output;
    for (int k = 0; k < size; k++) {
        double s = (k == 0) ? 1 / (double)sqrt(size) : (sqrt(((double)2 / size)));
        for (int n = 0; n < size; n++)
            *pout++ = s * cos((double)M_PI * (2 * n + 1) * k / (2 * size));
    }
    return output;
}

void Transform::_forward(TransformType type, float *input, float *output, Point4D &shape) {
    size_t axis_arr[4][3] = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};
    const float *pin = input;
    float *pout = output;

    for (int v = 0; v < shape.v; ++v) {
        for (int u = 0; u < shape.u; ++u) {
            for (int y = 0; y < shape.y; ++y) {
                _forward_1(type, pin, pout, stride.x, shape.x);
                pin += stride.y;
                pout += stride.y;
            }
            pin += (this->shape.y - shape.y) * stride.y;
            pout += (this->shape.y - shape.y) * stride.y;
        }
        pin += (this->shape.u - shape.u) * stride.u;
        pout += (this->shape.u - shape.u) * stride.u;
    }
    std::copy(output, output + flat_size, partial_values);
    pin = partial_values, pout = output;

    for (int v = 0; v < shape.v; ++v) {
        for (int u = 0; u < shape.u; ++u) {
            for (int x = 0; x < shape.x; ++x) {
                _forward_1(type, pin, pout, stride.y, shape.y);

                ++pin, ++pout;
            }
            pin += (stride.u - stride.y) + (this->shape.x - shape.x);
            pout += (stride.u - stride.y) + (this->shape.x - shape.x);
        }
        pin += (this->shape.u - shape.u) * stride.u;
        pout += (this->shape.u - shape.u) * stride.u;
    }

    std::copy(output, output + flat_size, partial_values);
    pin = partial_values, pout = output;

    for (int v = 0; v < shape.v; ++v) {
        for (int y = 0; y < shape.y; ++y) {
            for (int x = 0; x < shape.x; ++x) {
                _forward_1(type, pin, pout, stride.u, shape.u);

                ++pin, ++pout;
            }
            pin += (this->shape.x - shape.x);
            pout += (this->shape.x - shape.x);
        }
        pin += (stride.v - stride.u) + (this->shape.y - shape.y) * stride.y;
        pout += (stride.v - stride.u) + (this->shape.y - shape.y) * stride.y;
    }

    std::copy(output, output + flat_size, partial_values);
    pin = partial_values, pout = output;

    for (int u = 0; u < shape.u; ++u) {
        for (int y = 0; y < shape.y; ++y) {
            for (int x = 0; x < shape.x; ++x) {
                _forward_1(type, pin, pout, stride.v, shape.v);

                ++pin, ++pout;
            }
            pin += (this->shape.x - shape.x);
            pout += (this->shape.x - shape.x);
        }
        pin += (this->shape.y - shape.y) * stride.y;
        pout += (this->shape.y - shape.y) * stride.y;
    }
}

void Transform::_inverse(TransformType type, float *input, float *output, Point4D &shape) {
    size_t axis_arr[4][3] = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};

    float *pin = (float *)input;
    float *pout = output;

    for (int u = 0; u < shape.u; ++u) {
        for (int y = 0; y < shape.y; ++y) {
            for (int x = 0; x < shape.x; ++x) {
                _inverse_1(type, pin, pout, stride.v, shape.v);

                ++pin, ++pout;
            }
            pin += (this->shape.x - shape.x);
            pout += (this->shape.x - shape.x);
        }
        pin += (this->shape.y - shape.y) * stride.y;
        pout += (this->shape.y - shape.y) * stride.y;
    }

    std::copy(output, output + flat_size, partial_values);
    pin = partial_values, pout = output;

    for (int v = 0; v < shape.v; ++v) {
        for (int y = 0; y < shape.y; ++y) {
            for (int x = 0; x < shape.x; ++x) {
                _inverse_1(type, pin, pout, stride.u, shape.u);

                ++pin, ++pout;
            }
            pin += (this->shape.x - shape.x);
            pout += (this->shape.x - shape.x);
        }
        pin += (stride.v - stride.u) + (this->shape.y - shape.y) * stride.y;
        pout += (stride.v - stride.u) + (this->shape.y - shape.y) * stride.y;
    }
    std::copy(output, output + flat_size, partial_values);
    pin = partial_values, pout = output;
    for (int v = 0; v < shape.v; ++v) {
        for (int u = 0; u < shape.u; ++u) {
            for (int x = 0; x < shape.x; ++x) {
                _inverse_1(type, pin, pout, stride.y, shape.y);

                ++pin, ++pout;
            }
            pin += (stride.u - stride.y) + (this->shape.x - shape.x);
            pout += (stride.u - stride.y) + (this->shape.x - shape.x);
        }
        pin += (this->shape.u - shape.u) * stride.u;
        pout += (this->shape.u - shape.u) * stride.u;
    }
    std::copy(output, output + flat_size, partial_values);
    pin = partial_values, pout = output;
    for (int v = 0; v < shape.v; ++v) {
        for (int u = 0; u < shape.u; ++u) {
            for (int y = 0; y < shape.y; ++y) {
                _inverse_1(type, pin, pout, stride.x, shape.x);
                pin += stride.y;
                pout += stride.y;
            }
            pin += (this->shape.y - shape.y) * stride.y;
            pout += (this->shape.y - shape.y) * stride.y;
        }
        pin += (this->shape.u - shape.u) * stride.u;
        pout += (this->shape.u - shape.u) * stride.u;
    }
}

void Transform::flush_cache() {}

Transform::TransformType Transform::get_type(std::string transform) {
    if (transform == "DST")
        return DST;
    else if (transform == "DST_II")
        return DST_II;
    else if (transform == "DST_VII")
        return DST_VII;
    else if (transform == "DCT")
        return DCT;
    else if (transform == "DST_VII_2")
        return DST_VII_2;
    else
        std::cerr << "Unkown transform: " << transform << std::endl;
    return DCT;
}

void Transform::forward(TransformType transform, float *input, float *output, Point4D &shape) {
    float seg_block[this->flat_size];

    if (use_segments == NO_SEGMENTS) {
        if (axis_to_flip != NO_AXIS) flip_axis(output, axis_to_flip, flat_size, shape, stride);
        _forward(transform, input, output, shape);
    } else {
        auto shape_bak = this->shape;
        auto stride_bak = this->stride;
        auto flat_size_bak = this->flat_size;
        float seg_block[flat_size_bak];
        auto *pout = output;
        auto *pseg = seg_block;
        std::vector _shape {shape_bak.x, shape_bak.y, shape_bak.u, shape_bak.v};
        std::vector base_shape {(shape_bak.x >> use_segments) + (shape_bak.x & 1),
                                (shape_bak.y >> use_segments) + (shape_bak.y & 1),
                                (shape_bak.u >> use_segments) + (shape_bak.u & 1),
                                (shape_bak.v >> use_segments) + (shape_bak.v & 1)};
        segment_block(input, _shape, seg_block, base_shape);
        auto shapes = make_shapes(_shape, base_shape);
        int seg_count = 0;
        for (auto &curr_shape: shapes) {
            this->shape = Point4D(curr_shape.data());
            this->stride = make_stride(curr_shape);
            this->flat_size = this->shape.getNSamples();
            if (axis_to_flip != NO_AXIS)
                flip_axis(pout, axis_to_flip, flat_size, this->shape, stride);
            _forward(transform, pseg, pout, this->shape);
            pout += this->flat_size;
            pseg += this->flat_size;
        }
        this->shape = shape_bak;
        this->stride = stride_bak;
        this->flat_size = flat_size_bak;
    }
}

void Transform::inverse(TransformType transform, float *input, float *output, Point4D &shape) {
    float seg_block[this->flat_size];
    if (use_segments == NO_SEGMENTS) {
        _inverse(transform, input, output, shape);
        if (axis_to_flip != NO_AXIS) flip_axis(input, axis_to_flip, flat_size, shape, stride);
    } else {
        auto shape_bak = this->shape;
        auto stride_bak = this->stride;
        auto flat_size_bak = this->flat_size;
        auto *pout = seg_block;
        auto *pseg = input;
        std::vector _shape {shape_bak.x, shape_bak.y, shape_bak.u, shape_bak.v};
        std::vector base_shape {(shape_bak.x >> use_segments) + (shape_bak.x & 1),
                                (shape_bak.y >> use_segments) + (shape_bak.y & 1),
                                (shape_bak.u >> use_segments) + (shape_bak.u & 1),
                                (shape_bak.v >> use_segments) + (shape_bak.v & 1)};
        int seg_count = 0;
        auto shapes = make_shapes(_shape, base_shape);
        for (auto &curr_shape: shapes) {
            this->shape = Point4D(curr_shape.data());
            this->stride = make_stride(curr_shape);
            this->flat_size = this->shape.getNSamples();
            _inverse(transform, pseg, pout, this->shape);
            if (axis_to_flip != NO_AXIS)
                flip_axis(pseg, axis_to_flip, flat_size, this->shape, stride);
            pout += this->flat_size;
            pseg += this->flat_size;
        }

        join_segments(seg_block, _shape, output, base_shape);
        this->shape = shape_bak;
        this->stride = stride_bak;
        this->flat_size = flat_size_bak;
    }
}





template <typename T, typename size_type>
void Transform::segment_block(const T *from_block,
                              const std::vector<size_type> &from_shape,
                              T *into_block,
                              const std::vector<size_type> &base_shape) {
    auto *curr_seg = into_block;
    auto stride = make_stride(from_shape);
    int seg_index = 0;
    const auto shapes = make_shapes(from_shape, base_shape);
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
void Transform::join_segments(const T *from_block,
                              const std::vector<size_type> &from_shape,
                              T *into_block,
                              const std::vector<size_type> &base_shape) {
    auto *curr_seg = from_block;
    auto stride = make_stride(from_shape);
    int seg_index = 0;
    const auto shapes = make_shapes(from_shape, base_shape);
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
