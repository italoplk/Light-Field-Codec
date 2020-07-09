
#include "Transform.h"
#include "Quantization.h"
#include "EncBitstreamWriter.h"
#include "LRE.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include "utils.h"

std::map<size_t, float *> Transform::_DCT_II_CACHE;
std::map<size_t, float *> Transform::_DST_II_CACHE;
std::map<size_t, float *> Transform::_DST_VII_CACHE;

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
        case DCT_II:
        default: try { coeff = _DCT_II_CACHE.at(size);
            } catch (...) {
                coeff = _DCT_II(size);
                _DCT_II_CACHE[size] = coeff;
            }
    }
    return coeff;
}

void Transform::_forward_sd(Transform::TransformType type,
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

void Transform::_inverse_sd(Transform::TransformType type,
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
    this->enforce_transform = TransformType::NO_TRANSFORM;
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

void Transform::_forward_md(TransformType type, float *input, float *output, Point4D &shape) {
    size_t axis_arr[4][3] = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};
    const float *pin = input;
    float *pout = output;

    for (int v = 0; v < shape.v; ++v) {
        for (int u = 0; u < shape.u; ++u) {
            for (int y = 0; y < shape.y; ++y) {
                _forward_sd(type, pin, pout, stride.x, shape.x);
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
                _forward_sd(type, pin, pout, stride.y, shape.y);

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
                _forward_sd(type, pin, pout, stride.u, shape.u);

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
                _forward_sd(type, pin, pout, stride.v, shape.v);

                ++pin, ++pout;
            }
            pin += (this->shape.x - shape.x);
            pout += (this->shape.x - shape.x);
        }
        pin += (this->shape.y - shape.y) * stride.y;
        pout += (this->shape.y - shape.y) * stride.y;
    }
}

void Transform::_inverse_md(TransformType type, float *input, float *output, Point4D &shape) {
    size_t axis_arr[4][3] = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};

    float *pin = (float *)input;
    float *pout = output;

    for (int u = 0; u < shape.u; ++u) {
        for (int y = 0; y < shape.y; ++y) {
            for (int x = 0; x < shape.x; ++x) {
                _inverse_sd(type, pin, pout, stride.v, shape.v);

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
                _inverse_sd(type, pin, pout, stride.u, shape.u);

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
                _inverse_sd(type, pin, pout, stride.y, shape.y);

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
                _inverse_sd(type, pin, pout, stride.x, shape.x);
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
        return DST_II;
    else if (transform == "DST_II")
        return DST_II;
    else if (transform == "DST_VII")
        return DST_VII;
    else if (transform == "DCT")
        return DCT_II;
    else
        std::cerr << "Unkown transform: " << transform << std::endl;
    return DCT_II;
}

void Transform::use_statistics(const std::string filename) {
    stats_stream.open(filename);
    stats_stream << "segment" << sep << "channel" << sep << "pos_x" << sep << "pos_y" << sep
                 << "pos_u" << sep << "pos_v" << sep << "bl_x" << sep << "bl_y" << sep << "bl_u"
                 << sep << "bl_v" << sep << "v_min" << sep << "v_max" << sep << "v_minAbs" << sep
                 << "v_maxAbs" << sep << "zeros" << sep << "ones" << sep << "mean" << sep << "std"
                 << sep << "variance" << sep << "energy" << sep << std::endl;
}

void Transform::set_position(int channel, const Point4D &current_pos) {
    this->channel = channel;
    this->position = current_pos;
}

static inline float _max(float a, float b) { return a > b ? a : b; }

static inline float _min(float a, float b) { return a < b ? a : b; }

void Transform::calculate_metrics(const float *block, const Point4D &shape) {
    max = block[0];
    min = block[0];
    abs_max = std::abs(block[0]);
    abs_min = std::abs(block[0]);
    zeros = 0;
    ones = 0;
    energy = 0;
    sum = 0;
    count = 0;
    for (int v = 0; v < shape.v; v++)
        for (int u = 0; u < shape.u; u++)
            for (int y = 0; y < shape.y; y++)
                for (int x = 0; x < shape.x; x++) {
                    auto index = offset(x, y, u, v, stride);
                    max = _max(max, block[index]);
                    min = _min(min, block[index]);
                    abs_max = _max(abs_max, std::abs(block[index]));
                    abs_min = _min(abs_min, std::abs(block[index]));
                    if (std::abs(block[index]) < 0.5)
                        zeros++;
                    else if (std::abs(block[index]) - 1 < 0.5)
                        ones++;
                    energy += block[index] * block[index];
                    sum += block[index];
                    count++;
                }
}

void Transform::write_stats(const int segment, const float *block, const Point4D &shape) {
    if (!stats_stream.is_open()) return;

    calculate_metrics(block, shape);
    float countf = count;
    float mean = sum / countf;
    float variance = (1 / countf) * (energy - std::pow(sum / countf, 2));
    float std = std::sqrt(variance);
    stats_stream << segment << sep << channel << sep << position.x << sep << position.y << sep
                 << position.u << sep << position.v << sep << shape.x << sep << shape.y << sep
                 << shape.u << sep << shape.v << sep << min << sep << max << sep << abs_min << sep
                 << abs_max << sep << zeros << sep << ones << sep << mean << sep << std << sep
                 << variance << sep << energy << sep << std::endl;
}
auto Transform::get_quantization_procotol(TransformType transform) {
    switch (transform) {
        case DCT: return Quantization::DEFAULT;
        default: return Quantization::HOMOGENEOUS;
    }
}
static float mse(float *original, float *reconstructed, unsigned size) {
    auto value = 0;
    for (int i = 0; i < size; i++)
        value += std::pow(original[i] - reconstructed[i], 2);
    return value / size;
}



auto Transform::calculate_distortion(float *block, float *result, Point4D &shape) {
    shape.updateNSamples();
    const int SIZE = this->shape.getNSamples();
    std::vector<TransformType> transforms;

    if (enforce_transform == NO_TRANSFORM)
        return std::tuple(NO_TRANSFORM, std::numeric_limits<float>::infinity());

    else if (enforce_transform == ANY)
        for (const auto &type: ALL_TRANSFORMS)
            transforms.push_back(type);
    else
        transforms.push_back(enforce_transform);

    float blk_transf[SIZE];
    float blk_qnt[SIZE];
    float blk_iquant[SIZE];
    float blk_itransf[SIZE];

    std::fill(blk_transf, blk_transf + SIZE, 0);
    std::fill(blk_qnt, blk_qnt + SIZE, 0);
    std::fill(blk_iquant, blk_iquant + SIZE, 0);
    std::fill(blk_itransf, blk_itransf + SIZE, 0);

    float best_score = std::numeric_limits<float>::infinity();
    TransformType best_type;

    Quantization quantizer(shape, qp, quant_weight_100);
    for (const auto &type: transforms) {
        _forward_md(type, block, blk_transf, shape);
        quantizer.foward(get_quantization_procotol(type), blk_transf, blk_qnt);
        quantizer.inverse(get_quantization_procotol(type), blk_qnt, blk_iquant);
        _inverse_md(type, blk_iquant, blk_itransf, shape);
        auto curr_score = mse(block, blk_itransf, SIZE);

        if (curr_score < best_score) {
            best_type = type;
            best_score = curr_score;
            std::copy(blk_qnt, blk_qnt + SIZE, result);
        }
    }
    return std::tuple(best_type, best_score);
}

auto Transform::calculate_tree(float *block, float *result, Point4D &shape, int level) {
    if (!block) /* Dummy return to deduce return type */
        return std::tuple((_Node *)nullptr, 0.0F);

    const int SIZE = this->shape.getNSamples();
    shape.updateNSamples();

    float block_transformed[SIZE];
    float block_segmented[SIZE];
    float segments_results[SIZE];
    float joined_results[SIZE];

    std::fill(block_transformed, block_transformed + SIZE, 0);
    std::fill(block_segmented, block_segmented + SIZE, 0);
    std::fill(segments_results, segments_results + SIZE, 0);
    std::fill(joined_results, joined_results + SIZE, 0);

    float segments_score = std::numeric_limits<float>::infinity();
    auto block_shape = this->shape.to_vector();
    float *transformed_block;
    float final_score;

    auto [block_type, block_score] = calculate_distortion(block, block_transformed, shape);
    auto root = new _Node();

    if (level > 0) {
        float segments_score_sum = 0;
        auto shapes = make_shapes(block_shape, 1);

        auto shape_bak = this->shape;
        auto stride_bak = this->stride;
        auto flat_size_bak = this->flat_size;
        segment_block(block, block_shape, block_segmented, 1);
        float *pseg = block_segmented;
        float *pres = segments_results;

        for (int i = 0; i < shapes.size(); i++) {
            Point4D seg_shape(shapes[i].data());
            this->shape = seg_shape;
            this->flat_size = seg_shape.getNSamples();
            this->stride = make_stride(seg_shape);
            auto [node, score] = calculate_tree(pseg, pres, seg_shape, level - 1);

            root->set_child(i, node);
            segments_score_sum += score;
            pseg += seg_shape.getNSamples();
            pres += seg_shape.getNSamples();
        }
        this->shape = shape_bak;
        this->stride = stride_bak;
        this->flat_size = flat_size_bak;

        segments_score = segments_score_sum / shapes.size();
    }
    if (segments_score < block_score) {
        transformed_block = segments_results;
        final_score = segments_score;
    } else {
        root->set_transform_type(block_type);
        transformed_block = block_transformed;
        final_score = block_score;
    }
    std::copy(transformed_block, transformed_block + SIZE, result);
    return std::tuple(root, final_score);
}

void Transform::reconstruct_from_tree(_Node *root, float *input, float *output, Point4D &shape) {
    shape.updateNSamples();
    const int SIZE = this->shape.getNSamples();
    float block_iquant[SIZE];
    float block_segments[SIZE];
    float segment_results[SIZE];
    std::fill(block_iquant, block_iquant + SIZE, 0);
    std::fill(block_segments, block_segments + SIZE, 0);
    std::fill(segment_results, segment_results + SIZE, 0);

    if (root->transform_type != -1) {
        Quantization quantizer(shape, qp, quant_weight_100);
        quantizer.inverse(get_quantization_procotol((TransformType)root->transform_type), input,
                          block_iquant);
        _inverse_md((TransformType)root->transform_type, block_iquant, output, shape);
    } else {
        auto block_shape = this->shape.to_vector();
        auto shapes = make_shapes(block_shape, 1);
        auto shape_bak = this->shape;
        auto stride_bak = this->stride;
        auto flat_size_bak = this->flat_size;

        float *pseg = input;
        float *pres = segment_results;
        for (int i = 0; i < shapes.size(); i++) {
            Point4D seg_shape(shapes[i].data());
            this->shape = seg_shape;
            this->flat_size = seg_shape.getNSamples();
            this->stride = make_stride(seg_shape);
            reconstruct_from_tree(root->children[i], pseg, pres, seg_shape);
            pseg += seg_shape.getNSamples();
            pres += seg_shape.getNSamples();
        }
        this->shape = shape_bak;
        this->stride = stride_bak;
        this->flat_size = flat_size_bak;
        join_segments(segment_results, block_shape, output, 1);
    }
}

std::string
Transform::forward(TransformType transform, float *input, float *output, Point4D &shape) {
    shape.updateNSamples();
    char buffer[1 << 16];
#if !!LFCODEC_FORCE_DCT_NON_LUMA && USE_YCbCr == 1
    this->enforce_transform = channel == 0 ? transform : DCT_II;
#else
    this->enforce_transform = transform;
#endif
    const int MAX_LEVELS = disable_segmentation ? 0 : std::min(LFCODEC_SEGMENTATION_MAX_LEVELS, 3);
#if !!LFCODEC_USE_SEGMENTATION
#if !!LFCODEC_FORCE_DCT_NON_LUMA && USE_YCbCr == 1
    std::vector channel_mapping = {MAX_LEVELS, 0, 0};
#else
    std::vector channel_mapping = {MAX_LEVELS, MAX_LEVELS, MAX_LEVELS};
#endif  
#else
    std::vector channel_mapping = {0, 0, 0};
#endif
    auto [root, mse] = calculate_tree(input, output, shape, channel_mapping[channel]);
    root->to_string(buffer);
    std::string tree = buffer;
    delete root;
    return tree;
}

void Transform::inverse(TransformType transform, float *input, float *output, Point4D &shape) {
    shape.updateNSamples();
    const int SIZE = this->shape.getNSamples();
    float block_iquant[SIZE];
    Quantization quantizer(shape, qp, quant_weight_100);
    quantizer.inverse(get_quantization_procotol(transform), input, block_iquant);
    _inverse_md(transform, block_iquant, output, shape);
}

void Transform::inverse(const std::string tree, float *input, float *output, Point4D &shape) {
    _Node root;
    root.from_string(tree.c_str());
    reconstruct_from_tree(&root, input, output, shape);
}

