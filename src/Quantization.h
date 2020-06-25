
#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include <cmath>
#include "Typedef.h"
#include "Point4D.h"

class Quantization {
private:
    Point4D dim_block, weight_100;

    float *volume{nullptr};

#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
    float *volume_hom = nullptr;
#endif

    float qp;

    static float clip_min(float val, float min);

    void generateVolume();

public:
    Quantization(const Point4D &dimBlock, float qp, const Point4D &weight_100);

    void delete_volume();

    virtual ~Quantization();

    void inverse(const float *input, float *output);

    void foward(const float *input, float *output);


#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
    enum {
        DEFAULT = 0,
        HOMOGENEOUS
    };
    void inverse(const int volume, const float *input, float *output);

    void foward(const int volume, const float *input, float *output);
#endif
};


#endif //QUANTIZATION_H
