
#ifndef PREDICTION_H
#define PREDICTION_H

#include "Typedef.h"
#include "clip.h"
#include "Point4D.h"
#include <string>
#include <cstring>

class Prediction {

public:
    explicit Prediction();

    ~Prediction();

    LFSample *rgb[3];
    LFSample predictors[3];
    LFSample predictor;

    uint num_elementos;


    int mNumberOfHorizontalViews, mNumberOfVerticalViews;

    int mColumns, mLines;

    int start_t{0}, start_s{0};

    void predict(const float *orig_input, const Point4D &origSize, float *out );

    void getBlock(float *block, const Point4D &pos, const Point4D &dim_block, const Point4D &stride_block,
                  const Point4D &origSize, const Point4D &stride_lf, int channel);

    void rec(float *input, float *out, Point4D &dim_block);

};

#endif //PREDICTION_H
