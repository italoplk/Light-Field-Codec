#include "Quantization.h"

#include <cmath>

Quantization::Quantization(const Point4D &dimBlock, float qp, const Point4D &weight_100):
    dim_block(dimBlock),
    qp(qp),
    weight_100(weight_100) {
    this->generateVolume();
}

void Quantization::foward(const float *input, float *output) {
#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
    foward(DEFAULT, input, output);
#else
    const uint i_lim = this->dim_block.getNSamples();

    for (uint i = 0; i < i_lim; ++i)
        output[i] = std::trunc(input[i] / this->volume[i]);
#endif
}

void Quantization::inverse(const float *input, float *output) {
#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
    inverse(DEFAULT, input, output);
#else
    const uint i_lim = this->dim_block.getNSamples();

    for (uint i = 0; i < i_lim; ++i)
        output[i] = (((float)input[i]) * this->volume[i]);
#endif
}

#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
void Quantization::inverse(const int volume, const float *input, float *output) {
    const uint i_lim = this->dim_block.getNSamples();
    float *volume_matrix;
    switch (volume) {
        case HOMOGENEOUS: volume_matrix = this->volume_hom; break;
        case DEFAULT:
        default: volume_matrix = this->volume; break;
    }
    for (uint i = 0; i < i_lim; ++i)
        output[i] = (((float)input[i]) * volume_matrix[i]);
}

void Quantization::foward(const int volume, const float *input, float *output) {
    const uint i_lim = this->dim_block.getNSamples();
    float *volume_matrix;
    switch (volume) {
        case HOMOGENEOUS: volume_matrix = this->volume_hom; break;
        case DEFAULT:
        default: volume_matrix = this->volume; break;
    }
    for (uint i = 0; i < i_lim; ++i)
        output[i] = std::trunc(input[i] / volume_matrix[i]);
}
#endif

float Quantization::clip_min(float val, float min) { return (val > min) ? val : min; }

void Quantization::generateVolume() {
    const float weight_x = this->weight_100.x / 100.0f, weight_y = this->weight_100.y / 100.0f,
                weight_u = this->weight_100.u / 100.0f, weight_v = this->weight_100.v / 100.0f;

    float val;
    this->volume = new float[this->dim_block.getNSamples()];
    auto it_vol = this->volume;

#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
    this->volume_hom = new float[this->dim_block.getNSamples()];
    auto val_hom = this->qp * (weight_x + weight_y + weight_u + weight_v);
    auto it_vol_hom = this->volume_hom;
#endif

    for (int v = 0; v < this->dim_block.v; ++v) {
        for (int u = 0; u < this->dim_block.u; ++u) {
            for (int y = 0; y < this->dim_block.y; ++y) {
                for (int x = 0; x < this->dim_block.x; ++x) {
                    val = this->qp * (4 + (float)x * weight_x + (float)y * weight_y +
                                      (float)u * weight_u + (float)v * weight_v);

                    *it_vol++ = clip_min(val, 1.0);

#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
                    *it_vol_hom++ = val_hom;
#endif
                }
            }
        }
    }

    volume[0] = 1.0;
}

void Quantization::delete_volume() {
    delete[] this->volume;
#if LFCODEC_QUANTIZATION_EXTRA_VOLUMES
    delete[] this->volume_hom;
#endif
}

Quantization::~Quantization() { this->delete_volume(); }
