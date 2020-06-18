
#ifndef PREDICTION_H
#define PREDICTION_H

#include "Typedef.h"
#include "clip.h"
#include "Point4D.h"
#include <string>
#include <cstring>
//EDUARDO BEGIN
#include <vector>


class ValueBlockPred {
public:
    bool available{false};

    std::vector<float> block4D;

    ValueBlockPred() = default;

    ValueBlockPred(float *block4D, bool available, uint blockSize);

};
//EDUARDO END

class Prediction {

public:
    explicit Prediction();

    ~Prediction();

    LFSample *rgb[3];
    LFSample predictors[3];
    LFSample predictor;

    uint num_elementos;

    //EDUARDO BEGIN
    uint resol_x; //tamanho

    std::vector<ValueBlockPred> pred_references;

    void init_references();

    Prediction(uint resol_x);

    int get_reference(uint x, uint y);

    void update(float *curr, bool available, uint blockSize);

    void get_referenceL(uint x, uint y, float *out, const Point4D &origSize);

    void get_referenceA(uint x, uint y, float *out, const Point4D &origSize);

    void get_referenceLA(uint x, uint y, float *out, const Point4D &origSize);

    void predictRef(const float *orig_input, const float *ref, const Point4D &origSize, float *out );

    void angularPredictRefHorizontal(const float *orig_input, const float *ref, const Point4D &origSize, float *out );

    void angularPredictRefVertical(const float *orig_input, const float *ref, const Point4D &origSize, float *out );

    float sadHorizontal(const float *orig_input, const float *prediction_input, const Point4D &origSize);

    float sadVertical(const float *orig_input, const float *prediction_input, const Point4D &origSize);

    void recRef(const float *input, const Point4D &origSize, float *out );

    //const std::vector<ValueBlockPred> &getPredReferences() const;
    //EDUARDO END

    int mNumberOfHorizontalViews, mNumberOfVerticalViews;

    int mColumns, mLines;

    int start_t{0}, start_s{0};

    void predict(const float *orig_input, const Point4D &origSize, float *out );

    void getBlock(float *block, const Point4D &pos, const Point4D &dim_block, const Point4D &stride_block,
                  const Point4D &origSize, const Point4D &stride_lf, int channel);

    void rec(float *input, float *out, Point4D &dim_block);

};

#endif //PREDICTION_H
