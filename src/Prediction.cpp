#include "Prediction.h"

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