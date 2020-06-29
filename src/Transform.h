
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Point4D.h"
#include "Typedef.h"
#include "EncoderParameters.h"
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <deque>
#include <memory>


struct _Node {
    std::vector<_Node *> children;
    int transform_type;

    _Node() {
        children.resize(16);
        transform_type = -1;
    };

    ~_Node() {
        for (auto& it : children)
            if (it) delete it;
        
    }

    void set_child(int child_pos, _Node *node) {
        children[child_pos] = node;
    }

    void set_transform_type(int type) { transform_type = type; }

    void from_string(const char *tree) {
        if (*tree == '\0') return;
        else if (tree[1] == '\0') {
            transform_type = (int)(*tree - '0');
        } else {
            std::deque<_Node *> queue;
            queue.push_back(this);
            const char *c = tree;
            while (!queue.empty()) {
                auto *node = queue.front();
                queue.pop_front();
                for (int i = 0; i < 16; i++, c++) {
                    auto *child = new _Node();
                    if (*c == '0')
                        queue.push_back(child);
                    else
                        child->transform_type = (int)(*c - '0');
                    node->set_child(i, child);
                }
            }
        }
    }

    void to_string(char *buffer) {
        std::deque<_Node *> queue;
        queue.push_back(this);
        char *c = buffer;
        while (!queue.empty()) {
            auto *node = queue.front();
            queue.pop_front();
            if (node->transform_type != -1)
                *c++ = (char)(node->transform_type + '0');
            else
                for (int i = 0; i < 16; i++, c++) {
                    auto child = node->children[i];
                    if (child && child->transform_type == -1) {
                        *c = '0';
                        queue.push_back(child);
                    } else if (child) {
                        *c = (char)child->transform_type + '0';
                    }
                }
        }
        *c = '\0';
    }
};


class Transform {
  public:
    enum TransformType {
        NO_TRANSFORM = 0, /* Empty value */
        DCT,              /* Discrete Cosine Transform Type II */
        DST,              /* Discrete Sine Transform Type II */
        DST_II = DST,     /* Discrete Sine Transform Type II */
        DST_VII,          /* Discrete Sine Transform Type VII */
        ANY, 
    };

    enum {
        NO_SEGMENTS,
        SEGMENTS_8,
        SEGMENTS_4,
        SEGMENTS_2,
    };

    enum { NO_AXIS = 0, AXIS_X = 1, AXIS_Y = 2, AXIS_U = 4, AXIS_V = 8 };

    int use_segments = NO_SEGMENTS;
    int axis_to_flip = NO_AXIS;
    EncoderParameters codec_parameters;
    float qp;
    Point4D quant_weight_100;
    static void flush_cache();
    static TransformType get_type(std::string transform);

    Transform(Point4D &shape);
    ~Transform();

    void use_statistics(const std::string filename);
    void set_position(int channel, const Point4D &current_pos);

    // void forward(TransformType transform, float *input, float *output, Point4D &shape);
    // void inverse(TransformType transform, float *input, float *output, Point4D &shape);

    std::string forward(TransformType transform, float *input, float *output, Point4D &shape);
    void inverse(const std::string tree, float *input, float *output, Point4D &shape);

  private:
    Point4D shape;
    Point4D stride;
    Point4D position;
    int channel;
    size_t flat_size;
    size_t flat_p2;
    float *wh_partial_values;
    float *partial_values;
    float max;
    float min;
    float abs_max;
    float abs_min;
    int zeros;
    int ones;
    int count;
    float energy;
    float sum;
    TransformType enforce_transform;
    std::ofstream stats_stream;
    const std::string sep = ",";
    const std::vector<TransformType> ALL_TRANSFORMS = {DCT, DST_II, DST_VII};

    static std::map<size_t, float *> _DCT_II_CACHE;
    static std::map<size_t, float *> _DST_II_CACHE;
    static std::map<size_t, float *> _DST_VII_CACHE;

    static float *_DCT_II(size_t size);
    static float *_DST_II(size_t size);
    static float *_DST_VII(size_t size);

    static auto get_transform(TransformType type, bool is_inverse = false);
    static float *_get_coefficients(TransformType type, const size_t size);
    static void _forward_1(TransformType type,
                           const float *in,
                           float *out,
                           const size_t offset,
                           const size_t size);
    static void _inverse_1(TransformType type,
                           const float *in,
                           float *out,
                           const size_t offset,
                           const size_t size);

    void _forward(TransformType transform, float *input, float *output, Point4D &shape);
    void _inverse(TransformType transform, float *input, float *output, Point4D &shape);

    void write_stats(int segment, const float *block, const Point4D &shape);
    void calculate_metrics(const float *block, const Point4D &shape);

    auto calculate_distortion(float *block,  float *result, Point4D &shape);
    auto calculate_tree(float *block, float *result, Point4D &shape, int level);
    void reconstruct_from_tree(_Node *root, float *input, float *output, Point4D& shape);
    auto get_quantization_procotol(TransformType transform);
    float calculate_bistream(const float *block);
};

#endif // TRANSFORM_H
