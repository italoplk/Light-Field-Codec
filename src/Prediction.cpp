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
                        R0 = ref[(it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_in.u) +
                                        (i * origSize.x * origSize.y * origSize.u)];
                        R1 = ref[(it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_in.u) +
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
                        R0 = ref[(it_pos_out.x) + (it_pos_in.y) + (i * origSize.x * origSize.y) +
                                        (it_pos_in.v)];
                        R1 = ref[(it_pos_out.x) + (it_pos_in.y) + ((pos) * origSize.x * origSize.y) +
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

void Prediction::angularPredictRefHorizontalMI(const float *orig_input, const float *ref, const Point4D &origSize, float *out ){
    Point4D it_pos_in;
    Point4D it_pos_out;

    // Horizontal
    // fixed
    it_pos_in.x = origSize.x - 1;

    it_pos_in.u = 0;
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
                        R0 = ref[(it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_out.u * origSize.x * origSize.y) +
                                 (i * origSize.x * origSize.y * origSize.u)];
                        R1 = ref[(it_pos_in.x) + (it_pos_out.y * origSize.x) + (it_pos_out.u * origSize.x * origSize.y) +
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

void Prediction::residuePred(const float *orig_input, const float *pred, const Point4D &origSize, float *out ){
    for (int i = 0; i < origSize.getNSamples(); ++i){
        out[i] = orig_input[i] - pred[i];
    }
}

void Prediction::recResiduePred(const float *orig_input, const float *pred, const Point4D &origSize, float *out ){
    for (int i = 0; i < origSize.getNSamples(); ++i){
        out[i] = orig_input[i] + pred[i];
    }
}

void Prediction::YCbCR2RGB(float **yCbCr, const Point4D &origSize, float **rgb, int mPGMScale) {

    int cont = 0;
    int mFirstPixelPosition = 0;

    int N = 10;
    float pixel[3];
    double M[] = {1.000000000000000, 1.000000000000000, 1.000000000000000, 0,
                  -0.187330000000000, 1.855630000000000, 1.574800000000000,
                  -0.468130000000000, 0};


    double nd = (double) (1 << (N - 8));

    unsigned short clipval = (unsigned short) (1 << N) - 1;  // pow(2, N) - 1;

    double sval1 = 16 * nd;
    double sval2 = 219 * nd;
    double sval3 = 128 * nd;
    double sval4 = 224 * nd;


    for (int index_t = 0; index_t < origSize.v; index_t++) { //vertical angular
        for (int index_s = 0; index_s < origSize.u; index_s++) { //horizontal angular
            mFirstPixelPosition = cont * origSize.x * origSize.y;
            cont++;

            for (int pixelCount = 0; pixelCount < origSize.x * origSize.y; pixelCount++) {

                for (int icomp = 0; icomp < 3; icomp++) {
                    yCbCr[icomp][mFirstPixelPosition + pixelCount] =
                            yCbCr[icomp][mFirstPixelPosition + pixelCount] + (mPGMScale + 1) / 2;

                    if (icomp < 1) {
                        yCbCr[icomp][mFirstPixelPosition + pixelCount] = clip(
                                (yCbCr[icomp][mFirstPixelPosition + pixelCount] - sval1) / sval2, 0.0, 1.0);
                    } else {
                        yCbCr[icomp][mFirstPixelPosition + pixelCount] = clip(
                                (yCbCr[icomp][mFirstPixelPosition + pixelCount] - sval3) / sval4, -0.5, 0.5);
                    }

                }

                for (int icomp = 0; icomp < 3; icomp++) {

                    pixel[icomp] = yCbCr[0][mFirstPixelPosition + pixelCount] * M[icomp + 0]
                                   + yCbCr[1][mFirstPixelPosition + pixelCount] * M[icomp + 3]
                                   + yCbCr[2][mFirstPixelPosition + pixelCount] * M[icomp + 6];

                    rgb[icomp][mFirstPixelPosition + pixelCount] = clip(
                            double(pixel[icomp] * clipval), 0.0, (double) clipval);
                }
            }
        }
    }
}

void Prediction::write(float **rgb, const Point4D &origSize, int mPGMScale, int start_t, int start_s, int block) {
    //this->YCbCR2RGB();

    //int cont = 0;
    //int cont2 = 0;

    //for (int index_t = start_t; index_t < start_t + origSize.v; index_t++) {
        //for (int index_s = start_s; index_s < start_s + origSize.u; index_s++) {

            /*
            std::string indice_t = std::to_string(index_t);
            std::string indice_s = std::to_string(index_s);
            indice_t = std::string(3 - indice_t.length(), '0') + indice_t;
            indice_s = std::string(3 - indice_s.length(), '0') + indice_s;

            std::string name_ppm = path + indice_t + "_" + indice_s + ".ppm";
            */

            std::string blockS = std::to_string(block);

            //printf("Opening View %s \n", name_ppm.c_str());
            FILE *mViewFilePointer = fopen(blockS.c_str(), "w");
            if (mViewFilePointer == nullptr) {
                printf("unable to open %s view file for writing\n", blockS.c_str());
                //assert(false);
            }

            //fwrite(fullTag, sizeof(char), sizeof(fullTag), mViewFilePointer);


            int mNumberOfFileBytesPerPixelComponent = (mPGMScale <= 255 ? 1 : 2);

            fprintf(mViewFilePointer, "P6\n%d %d\n%d\n", origSize.x * origSize.u, origSize.y * origSize.v, mPGMScale);

            Point4D it_pos;


            for (it_pos.y = 0; it_pos.y < origSize.y; it_pos.y += 1) {
                for (it_pos.v = 0; it_pos.v < origSize.v; it_pos.v += 1) {
                    for (it_pos.x = 0; it_pos.x < origSize.x; it_pos.x += 1) {
                        for (it_pos.u = 0; it_pos.u < origSize.u; it_pos.u += 1) {

                            int pos_out = (it_pos.x) + (it_pos.y * origSize.x) + (it_pos.u * origSize.x * origSize.y)
                                          + (it_pos.v * origSize.x * origSize.y * origSize.u);
                            WritePixelToFile(pos_out, rgb, mPGMScale, mNumberOfFileBytesPerPixelComponent, mViewFilePointer);
                        }
                    }
                }
            }

            /*
            mFirstPixelPosition = cont * mColumns * mLines;
            for (int pixelCount = 0; pixelCount < mColumns * mLines; pixelCount++) {
                WritePixelToFile(pixelCount);
            }

            cont++;
            */
        //}
    //}
}

void Prediction::WritePixelToFile(int pixelPositionInCache, float **rgb, int mPGMScale, int mNumberOfFileBytesPerPixelComponent, FILE *mViewFilePointer) {

    for (int component_index = 0; component_index < 3; component_index++) {
        int ClippedPixelValue = rgb[component_index][pixelPositionInCache];
        if (ClippedPixelValue > mPGMScale)
            ClippedPixelValue = mPGMScale;
        if (ClippedPixelValue < 0)
            ClippedPixelValue = 0;
        unsigned short bigEndianPixelValue = (mNumberOfFileBytesPerPixelComponent == 2) ? change_endianness_16b(
                ClippedPixelValue) : ClippedPixelValue;

        fwrite(&bigEndianPixelValue, mNumberOfFileBytesPerPixelComponent, 1, mViewFilePointer);
    }

}

unsigned short Prediction::change_endianness_16b(unsigned short val) {
    return (val << 8u) | ((val >> 8u) & 0x00ff);
}

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