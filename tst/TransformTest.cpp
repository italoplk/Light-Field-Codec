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

/* Helper function to return the representation of two floats if they 
 * are different. Debug porpouses.
 * TODO: Move all this to a logger!
 */
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


/**  
 * This tests if the new implementation is identical to the old one.
 * The test passes if both the DCT transform and its inverse are identical.
 * BUGS: Somehow, the old implementation used double as an intermediate value.
 * It caused small changes that, if accumulated, grew beyond 1e-5 in arrays for 
 * 100 elements.  
 */
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
    
    Transform t(size);
    ctx = new DiscreteCosineTransformContext<float>(ARRAY_1D_LEN);

    // Forward DCT
    ctx->forward(input_new, output_new);
    t.dct_4d(input_old, output_old, size, size);

    for (int i = 0; i < ARRAY_1D_LEN; i++) {
        ASSERT_FLOAT_EQ(output_new[i], output_old[i]) 
            << "DCT(" << i << "): " 
            << display_differences(output_new[i], output_old[i]).str();
    }

    std::swap(input_new, output_new);
    std::swap(output_old, input_old);
    
    // Inverse DCT
    ctx->inverse(input_new, output_new);
    t.idct_4d(input_old, output_old, size, size);

    for (int i = 0; i < ARRAY_1D_LEN; i++) 
        ASSERT_FLOAT_EQ(output_new[i], output_old[i]) 
            << "Inverse DCT(" << i << "): " 
            << display_differences(output_new[i], output_old[i]).str();
    
}

