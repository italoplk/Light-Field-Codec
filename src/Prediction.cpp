#include "Prediction.h"
//EDUARDO BEGIN
#include <math.h>
//EDUARDO END

Prediction::Prediction(){
    this->predictor = 0;
    this->predictors[0] = 0;
    this->predictors[1] = 0;
    this->predictors[2] = 0;
}

Prediction::~Prediction(){

}

//Preditor atual é apenas uma média do LF.
void Prediction::predict(const float *orig_input, const Point4D &origSize, float *out ){
    this->predictor = 0;

    this->num_elementos = origSize.getNSamples();

    for (int i = 0; i < this->num_elementos; ++i)
        this->predictor += orig_input[i];

    this->predictor = this->predictor/this->num_elementos;

    for (int i = 0; i < this->num_elementos; ++i){
        out[i] = orig_input[i] - this->predictor;
    }
}

void Prediction::rec(float *input, float *out, Point4D &dim_block){
    for (int i = 0; i < this->num_elementos ; ++i)
        out[i] = input[i] + this->predictor;
}


//EDUARDO BEGIN
ValueBlockPred::ValueBlockPred(float *block4D, bool available, uint blockSize){
    this->available = available;
    for(int i = 0; i < blockSize ; ++i){
        this->block4D.push_back(block4D[i]);
    }
}

Prediction::Prediction(uint resol_x) : resol_x(resol_x) {
    this->init_references();
}

void Prediction::get_referenceL(uint x, uint y, float *out, const Point4D &origSize) {
    ValueBlockPred ref = *(this->pred_references.end() - 1); // left
    int numElements = origSize.getNSamples();
    if (x == 0) ref.available = false;
    if(ref.available){
        for (int i = 0; i < numElements ; ++i)
            out[i] = ref.block4D[i];
    }else{
        for (int i = 0; i < numElements ; ++i)
            out[i] = 0;
    }
}

void Prediction::get_referenceA(uint x, uint y, float *out, const Point4D &origSize) {
    ValueBlockPred ref = *(this->pred_references.begin() + 1); // above
    int numElements = origSize.getNSamples();
    if (y == 0) ref.available = false;
    if(ref.available){
        for (int i = 0; i < numElements ; ++i)
            out[i] = ref.block4D[i];
    }else{
        for (int i = 0; i < numElements ; ++i)
            out[i] = 0;
    }
}

void Prediction::get_referenceLA(uint x, uint y, float *out, const Point4D &origSize) {
    ValueBlockPred ref = *this->pred_references.begin(); // left above
    int numElements = origSize.getNSamples();
    if (y == 0) ref.available = false;
    if (x == 0) ref.available = false;
    if(ref.available){
        for (int i = 0; i < numElements ; ++i)
            out[i] = ref.block4D[i];
    }else{
        for (int i = 0; i < numElements ; ++i)
            out[i] = 0;
    }
}

void Prediction::predictRef(const float *orig_input, const float *ref, const Point4D &origSize, float *out ){
    this->predictor = 0;
    int numElements = origSize.getNSamples();

    for (int i = 0; i < numElements; ++i)
        this->predictor += ref[i];

    this->predictor = this->predictor/numElements;

    for (int i = 0; i < numElements; ++i)
        out[i] = orig_input[i] - this->predictor;

}

void Prediction::angularPredictRefHorizontal(const float *orig_input, const float *ref, const Point4D &origSize, float *out ){
    Point4D it_pos_in;
    Point4D it_pos_out;

    // Horizontal - fixed
    it_pos_in.x = origSize.x - 1;
    it_pos_in.u = floor(origSize.u / 2) * origSize.x * origSize.y;

    // Vertical - variable
    it_pos_in.v = 0;
    it_pos_in.y = 0;

    it_pos_out.x = 0;
    it_pos_out.u = 0;
    it_pos_out.v = 0;
    it_pos_out.y = 0;

    // Horizontal modo 10 - H0
    int d = 0;
    int Cu = 0;
    int i = 0;
    int Wu = 0;
    float R0 = 0;
    float R1 = 0;

    int ref0 = 0;
    for(int i = 0; i < origSize.getNSamples(); i++){
         ref0 += ref[i];
    }

    if(ref0 == 0){
        for(int i = 0; i < origSize.getNSamples(); i++){
            out[i] = orig_input[i];
        }
    } else{ //se tem bloco de referência

        // percorre vetor out na ordem horizontal espacial
        for (it_pos_out.y = 0; it_pos_out.y < origSize.y; it_pos_out.y += 1) {
            for (it_pos_out.x = 0; it_pos_out.x < origSize.x; it_pos_out.x += 1) {

                // percorre vetor out na ordem horizontal angular
                for (it_pos_out.v = 0; it_pos_out.v < origSize.v; it_pos_out.v += 1) {
                    for (it_pos_out.u = 0; it_pos_out.u < origSize.u; it_pos_out.u += 1) {

                        int pos_out = (it_pos_out.x) + (it_pos_out.y * origSize.x) + (it_pos_out.u * origSize.x * origSize.y)
                                + (it_pos_out.v * origSize.x * origSize.y * origSize.u);

                        Cu = (it_pos_out.u * d) >> 5;
                        Wu = (it_pos_out.u * d) & 31;
                        i = it_pos_out.v + Cu;
                        int pos = i+1;
                        if (pos >= origSize.v){
                            pos = i;
                        }
                        R0 = orig_input[(it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_in.u) +
                                        (i * origSize.x * origSize.y * origSize.u)];
                        R1 = orig_input[(it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_in.u) +
                                        ((pos) * origSize.x * origSize.y * origSize.u)];

                        //Pu,v = ((32 - Wu)) * R0,i + Wu * R0,i + 1 + 16) >> 5;
                        out[pos_out] = ((32 - Wu) * R0 + Wu * R1 + 16) / pow(2, 5);

                        /*
                        printf("\nCu = %d * %d = %d", it_pos_out.u, d, Cu);
                        printf("\nWu = %d * %d = %d", it_pos_out.u, d, Wu);
                        printf("\ni = %d + %d = %d", it_pos_out.v, Cu, i);
                        printf("\nR0 - %d = %f", (it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_in.u) +
                                      (i * origSize.x * origSize.y * origSize.u), R0);
                        printf("\nR1 - %d = %f", (it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_in.u) +
                                             ((pos) * origSize.x * origSize.y * origSize.u), R1);
                        printf("\nOut - %d = %f", pos_out, out[pos_out]);
                        */
                    }
                }
            }
        }

    }

    /*
    // Horizontal
    Cu = (u * d) >> 5
    Wu = (u * d) & 31
    i = v + Cu
    Pu,v = ((32 - Wu)) * R0,i + Wu * R0,i + 1 + 16) >> 5

    Px,y = ((32 − wy) · Ri,0 + wy · Ri + 1,0 + 16) >> 5
    cy = (y · d) >> 5
    wy = (y · d) & 31
    i = x + cy
    */
}

void Prediction::angularPredictRefVertical(const float *orig_input, const float *ref, const Point4D &origSize, float *out ){
    Point4D it_pos_in;
    Point4D it_pos_out;

    // Horizontal - variable
    it_pos_in.x = 0;
    it_pos_in.u = 0;

    // Vertical - fixed
    it_pos_in.y = (origSize.y - 1) * origSize.x;
    it_pos_in.v = floor(origSize.v / 2) * origSize.x * origSize.y * origSize.u;

    it_pos_out.x = 0;
    it_pos_out.u = 0;
    it_pos_out.v = 0;
    it_pos_out.y = 0;

    // Vertical modo 26 - V0
    int d = 0;
    int Cv = 0;
    int i = 0;
    int Wv = 0;
    float R0 = 0;
    float R1 = 0;

    int ref0 = 0;
    for(int i = 0; i < origSize.getNSamples(); i++){
        ref0 += ref[i];
    }

    if(ref0 == 0){
        for(int i = 0; i < origSize.getNSamples(); i++){
            out[i] = orig_input[i];
        }
    } else{ //se tem bloco de referência

        // percorre vetor out na ordem vertical espacial
        for (it_pos_out.x = 0; it_pos_out.x < origSize.x; it_pos_out.x += 1) {
            for (it_pos_out.y = 0; it_pos_out.y < origSize.y; it_pos_out.y += 1) {

                // percorre vetor out na ordem vertical angular
                for (it_pos_out.u = 0; it_pos_out.u < origSize.u; it_pos_out.u += 1) {
                    for (it_pos_out.v = 0; it_pos_out.v < origSize.v; it_pos_out.v += 1) {

                        int pos_out = (it_pos_out.x) + (it_pos_out.y * origSize.x) + (it_pos_out.u * origSize.x * origSize.y)
                                + (it_pos_out.v * origSize.x * origSize.y * origSize.u);

                        Cv = (it_pos_out.v * d) >> 5;
                        Wv = (it_pos_out.v * d) & 31;
                        i = it_pos_out.u + Cv;
                        int pos = i+1;
                        if (pos >= origSize.v){
                            pos = i;
                        }
                        R0 = orig_input[(it_pos_out.x) + (it_pos_in.y) + (i * origSize.x * origSize.y) +
                                        (it_pos_in.v)];
                        R1 = orig_input[(it_pos_out.x) + (it_pos_in.y) + ((pos) * origSize.x * origSize.y) +
                                        (it_pos_in.v)];

                        //Pu,v = ((32 - Wv)) * Ri,0 + Wv * Ri + 1,0 + 16) >> 5
                        out[pos_out] = ((32 - Wv) * R0 + Wv * R1 + 16) / pow(2, 5);
                    }
                }
            }
        }
    }

    /*
    // Vertical
    Cv = (v * d) >> 5
    Wv = (v * d) & 31
    i = u + Cv
    Pu,v = ((32 - Wv)) * Ri,0 + Wv * Ri + 1,0 + 16) >> 5

    Px,y = ((32 − wy) · Ri,0 + wy · Ri + 1,0 + 16) >> 5
    cy = (y · d) >> 5
    wy = (y · d) & 31
    i = x + cy
    */
}

float Prediction::sadHorizontal(const float *orig_input, const float *prediction_input, const Point4D &origSize){
    Point4D it_pos;

    // Horizontal
    it_pos.x = origSize.x - 1;
    it_pos.u = floor(origSize.u / 2);

    it_pos.y = 0;
    it_pos.v = 0;

    float sum = 0;
    int pos = 0;

    for (it_pos.y = 0; it_pos.y < origSize.y; it_pos.y += 1) {

            for (it_pos.v = 0; it_pos.v < origSize.v; it_pos.v += 1) {

                pos = (it_pos.x) + (it_pos.y * origSize.x) + (it_pos.u * origSize.x * origSize.y)
                          + (it_pos.v * origSize.x * origSize.y * origSize.u);
                sum += abs(orig_input[pos] - prediction_input[pos]);
            }
    }
    return sum;
}

float Prediction::sadVertical(const float *orig_input, const float *prediction_input, const Point4D &origSize){
    Point4D it_pos;

    // Horizontal - variable
    it_pos.x = 0;
    it_pos.u = 0;

    // Vertical - fixed
    it_pos.y = origSize.y - 1;
    it_pos.v = floor(origSize.v / 2);

    float sum = 0;
    int pos = 0;

    for (it_pos.x = 0; it_pos.x < origSize.x; it_pos.x += 1) {

        for (it_pos.u = 0; it_pos.u < origSize.u; it_pos.u += 1) {

            pos = (it_pos.x) + (it_pos.y * origSize.x) + (it_pos.u * origSize.x * origSize.y)
                  + (it_pos.v * origSize.x * origSize.y * origSize.u);
            sum += abs(orig_input[pos] - prediction_input[pos]);
        }
    }
    return sum;
}

/*
float Prediction::sad(const float *orig_input, const float *prediction_input, const Point4D &origSize){
    float sum = 0;
    for (int i = 0; i < origSize.getNSamples(); ++i){
        sum += abs(orig_input[i] - prediction_input[i]);
        //printf("%d\n",  i);
    }
    return sum;
}*/

void Prediction::recRef(const float *input, const Point4D &origSize, float *out ){
    int numElements = origSize.getNSamples();

    for (int i = 0; i < numElements; ++i)
        out[i] = input[i] + this->predictor;
}

void Prediction::update(float *curr, bool available, uint blockSize) {
    this->pred_references.erase(this->pred_references.begin());
    this->pred_references.emplace_back(curr, available, blockSize);
}

void Prediction::init_references() {
    for (int i = 0; i < this->resol_x + 1; ++i) this->pred_references.emplace_back();
}
//EDUARDO END