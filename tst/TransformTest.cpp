#include <gtest/gtest.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdlib>

#include "../src/Transform.h"
#include "../src/TransformContext.h"
#include "../src/DiscreteCosineTransformContext.h"


#define ARRAY_1D_LEN    11
#define ERROR_EPSILON   1.0e-10


template <typename T>
T abs(T val) {
    if (val < 0)
        return -val;
    else
        return val;
}
template <typename T>
T error(const T *a1, const T *a2, size_t length) {
    T error_sum = 0;
    for(auto i = 0; i < length; ++i)
        error_sum += abs<T>(a1[i] - a2[i]);
    return error_sum;
}


template <typename T>
void swap(T* a, T* b) {
    T tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}


TEST(DCTContextTest, EnsureDCTOn1DIsTheSameAsOldAPI) {
    TransformContext<float> *ctx;

    float input[ARRAY_1D_LEN];
    float output_gt[ARRAY_1D_LEN]; 
    float output[ARRAY_1D_LEN];

    // Seed the random number generator
    std::srand(0);

    // Populate input with random numbers
    for (int i = 0; i < ARRAY_1D_LEN; i++) 
        input[i] = 256 * std::rand() / (RAND_MAX + 1);
    
    
    ctx = new DiscreteCosineTransformContext<float>(ARRAY_1D_LEN);
    ctx->forward(input, output);

    auto coeff = Transform::generate_dct_coeff(ARRAY_1D_LEN);
    Transform::dct_1D(input, output_gt, coeff, 1, ARRAY_1D_LEN);
    
    ASSERT_NEAR(error<float>(output, output_gt, ARRAY_1D_LEN), 0, ERROR_EPSILON);
}

TEST(DCTContextTest, InverseTransformBelowEpsilon) {
    TransformContext<float> *ctx;

    float input_new[ARRAY_1D_LEN];
    float input_old[ARRAY_1D_LEN];
    float output_new[ARRAY_1D_LEN];
    float output_old[ARRAY_1D_LEN];

    // Seed the random number generator
    std::srand(0);

    // Populate input with random numbers
    for (int i = 0; i < ARRAY_1D_LEN; i++) 
        input_new[i] = input_old[i] = 256 * std::rand() / (RAND_MAX + 1);
    
    // New API DCT-1D
    ctx = new DiscreteCosineTransformContext<float>(ARRAY_1D_LEN);
    ctx->forward(input_new, output_new);
    std::swap(input_new, output_new);
    ctx->inverse(input_new, output_new);

    // Old API DCT-1D
    auto coeff = Transform::generate_dct_coeff(ARRAY_1D_LEN);
    Transform::dct_1D(input_old, output_old, coeff, 1, ARRAY_1D_LEN);
    std::swap(input_new, output_new);
    Transform::idct_1D(input_old, output_old, coeff, 1, ARRAY_1D_LEN);

    ASSERT_NEAR(error<float>(output_new, output_old, ARRAY_1D_LEN), 0, ERROR_EPSILON);
}



// TEST(TransformContextTesting, CanCreateContext) {
//     TransformContext *ctx;
//     Point4D size(3,3,3,3), stride(1,3,9,27); 

//     TransformEnum DCT = TransformEnum::DCT;

//     ctx = new TransformContext(DCT, size.to_array(), stride.to_array());

//     ASSERT_EQ(ctx->transform, DCT);
//     ASSERT_EQ(Point4D(ctx->size), size);
//     ASSERT_EQ(Point4D(ctx->stride), stride);
// }

// TEST(DiscreteCosineTransformTesting, )

// TEST(TransformInterfaceTesting, PerformsForwardDCT) {
//     TransformInterface *t;
//     TransformEnum DCT;
//     TransformContext *ctx;

//     Point4D size(10,10,10,10);
//     Point4D stride(1,10,100,1000);

    
//     float *input = new float[size.getNSamples()];
//     float *output = new float[size.getNSamples()];
//     float *output_groundtruth = new float[size.getNSamples()];

//     // Initialize block.
//     for (int i = 0; i < size.getNSamples(); i++)
//         input[i] = ((float)i) / 10000;
    
//     // Load groundtruth from disk.
//     std::ifstream file("resources/dct_results_1.bin",
//                        std::ios::in | std::ios::binary);
//     ASSERT_TRUE(file.is_open());
//     file.seekg(0, std::ios::beg);
//     file.read((char *)output_groundtruth, sizeof(float) * size.getNSamples());
//     file.close();

//     // Real test begins here!
//     DCT = TransformEnum::DCT;
//     t = new TransformInterface();
//     ctx = new DiscreteCosineTransformContext(
//         size.to_array(),
//         stride.to_array()
//     );
//     t->forward(*ctx, input, output);

//     // Verify the values are correct
//     for (int i = 0; i < 10000; i++)
//         ASSERT_EQ(output[i], output_groundtruth[i]);
// }

// TEST(Point4DInterface, ReturnsArrayOfFloats) {
//     Point4D p(5,6,7,8);
//     float *arr = p.to_array();
//     // Values are stored correctly on object properties
//     ASSERT_EQ(p.x, 5);
//     ASSERT_EQ(p.y, 6);
//     ASSERT_EQ(p.u, 7);
//     ASSERT_EQ(p.v, 8);

//     // The correct values are returned as an array.
//     ASSERT_EQ(arr[X_AXIS], 5);
//     ASSERT_EQ(arr[Y_AXIS], 6);
//     ASSERT_EQ(arr[U_AXIS], 7);
//     ASSERT_EQ(arr[V_AXIS], 8);
// }
