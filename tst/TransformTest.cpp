#include <gtest/gtest.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>

#include "../src/Transform.h"
#include "../src/TransformContext.h"
#include "../src/DiscreteCosineTransformContext.h"
#include "../src/Point4D.h"

typedef union {
  float f;
  struct {
    unsigned int mantisa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } parts;
} float_cast;

#define ARRAY_1D_LEN    12548

std::stringstream display_differences(float _a, float _b) {
    std::stringstream ss;
    if (_a != _b) {
        float_cast a = { .f = _a };
        float_cast b = { .f = _b };
        ss << "A(" << (a.parts.sign ? '-' : '+') << ", "
                            << "e=" << a.parts.exponent << ", "
                            << "m=" << a.parts.mantisa << ") ";
        ss << "B(" << (b.parts.sign ? '-' : '+') << ", "
                            << "e=" << b.parts.exponent << ", "
                            << "m=" << b.parts.mantisa << ")";
    }
    return ss;
}


TEST(DCTContextTest, new_idct_has_same_results_as_old_api) {
    TransformContext<float> *ctx;

    float input_new[ARRAY_1D_LEN];
    float input_old[ARRAY_1D_LEN];
    float output_new[ARRAY_1D_LEN];
    float output_old[ARRAY_1D_LEN];
    Point4D size(ARRAY_1D_LEN, 1, 1, 1);

    // Seed the random number generator
    std::srand(0);

    // Populate input with random numbers
    for (int i = 0; i < ARRAY_1D_LEN; i++) {
        float value = 256 * (std::rand() / (RAND_MAX * 1.0));
        input_new[i] = value;
        input_old[i] = value;
        output_new[i] = 0;
        output_old[i] = 0;

    }
    
    // New API DCT-1D
    ctx = new DiscreteCosineTransformContext<float>(ARRAY_1D_LEN);
    Transform t(size);
    ctx->forward(input_new, output_new);
    t.dct_4d(input_old, output_old, size, size);

    for (int i = 0; i < ARRAY_1D_LEN; i++) {
        ASSERT_FLOAT_EQ(output_new[i], output_old[i]) << "DCT(" << i << "): " << display_differences(output_new[i], output_old[i]).str();
    }

    std::swap(input_new, output_new);
    std::swap(output_old, input_old);

    ctx->inverse(input_new, output_new);
    t.idct_4d(input_old, output_old, size, size);

    for (int i = 0; i < ARRAY_1D_LEN; i++) {
        ASSERT_FLOAT_EQ(output_new[i], output_old[i]) << "IDCT(" << i << "): " << display_differences(output_new[i], output_old[i]).str();
    }
}


// TEST(DCTContextTest, new_idct_has_same_results_as_old_api2) {
//     TransformContext<float> *ctx;

//     float input_new[ARRAY_1D_LEN];
//     float input_old[ARRAY_1D_LEN];
//     float output_new[ARRAY_1D_LEN];
//     float output_old[ARRAY_1D_LEN];

//     Point4D size(ARRAY_1D_LEN, 1, 1, 1);

//     // Seed the random number generator
//     std::srand(0);

//     // Populate input with random numbers
//     for (int i = 0; i < ARRAY_1D_LEN; i++) 
//         input_new[i] = input_old[i] = 256 * (std::rand() / (RAND_MAX * 1.0));
    
//     // New API DCT-1D
//     ctx = new DiscreteCosineTransformContext<float>(ARRAY_1D_LEN);
//     ctx->forward(input_new, output_new);
//     std::swap(input_new, output_new);
//     ctx->inverse(input_new, output_new);

//     // Old API DCT-1D
//     Transform t(size);
//     t.dct_4d(input_old, output_old, size, size);
//     std::swap(output_old, input_old);
//     t.idct_4d(input_old, output_old, size, size);

//     ASSERT_NEAR(distance<float>(output_new, output_old, ARRAY_1D_LEN), 0, ERROR_EPSILON);
// }


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
