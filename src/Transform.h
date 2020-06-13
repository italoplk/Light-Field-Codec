
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Point4D.h"
#include "Typedef.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

class Transform {
  public:
    enum TransformType {
        DCT,          /* Discrete Cosine Transform Type II */
        DST,          /* Discrete Sine Transform Type II */
        DST_II = DST, /* Discrete Sine Transform Type II */
        DST_VII,      /* Discrete Sine Transform Type VII */
        DST_VII_2,    /* Mix of DST-VII and DCT */
        FWHT,         /* Fast Welsh Hadamard Transform */
        BESTMATCH,    /* Defaults to DCT */
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

    Transform(Point4D &shape);
    ~Transform();

    static void flush_cache();
    static TransformType get_type(std::string transform);

    void forward(TransformType transform, float *input, float *output, Point4D &shape);

    void inverse(TransformType transform, float *input, float *output, Point4D &shape);

  private:
    Point4D shape;
    Point4D stride;
    size_t flat_size;
    size_t flat_p2;
    float *wh_partial_values;
    float *partial_values;

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

    template <typename T, typename size_type>
    void segment_block(const T *from_block,
                       const std::vector<size_type> &from_shape,
                       T *into_block,
                       const std::vector<size_type> &base_shape);
    template <typename T, typename size_type>
    void join_segments(const T *from_block,
                       const std::vector<size_type> &from_shape,
                       T *into_block,
                       const std::vector<size_type> &base_shape);
};


#endif // TRANSFORM_H
