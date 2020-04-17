#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>

// Old API
#include "../src/Point4D.h"
#include "../src/deprecated/Transform.h"

// New API
#include "../src/TransformContext.h"
#include "../src/DiscreteCosineTransformContext.h"
#include "../src/DiscreteCosineTransformContext4D.h"


#define ERROR_EPSILON 1e-6

auto gtestlog = spdlog::basic_logger_mt("gtestlog", "logs/gtestlog.txt");


/* helper union to extract float values. */
typedef union {
  float f;
  struct {
    unsigned int mantisa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } parts;
} float_cast;

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

/* Euclidean distance between two arrays */
template <typename T>
double distance(const T *a, const T *b, size_t length) 
{
    double _distance = 0;
    for (size_t i = 0; i < length; i++) 
        _distance += pow(a[i] - b[i], 2);
    return sqrt(_distance);
}


/**  
 * This tests if the new implementation is identical to the old one.
 * The test passes if both the DCT transform and its inverse are identical.
 * BUGS: Somehow, the old implementation used double as an intermediate value.
 * It caused small changes that, if accumulated, grew beyond 1e-5 in arrays for 
 * 100 elements.  
 */
TEST(ContextTest, new_dct_api_is_consistent_to_old_api) {
    #define FULL_LENGTH 512 
    TransformContext<float> *ctx;

    float input_new[FULL_LENGTH];
    float input_old[FULL_LENGTH];
    float output_new[FULL_LENGTH];
    float output_old[FULL_LENGTH];
    Point4D size(FULL_LENGTH, 1, 1, 1);

    // Seed the random number generator
    std::srand(0);

    // Populate input with random numbers and initialize output arrays.
    for (int i = 0; i < FULL_LENGTH; i++) {
        float value = 256 * (std::rand() / (RAND_MAX * 1.0));
        input_new[i] = value;
        input_old[i] = value;
        output_new[i] = 0;
        output_old[i] = 0;
    }
    
    old::Transform t(size);
    ctx = new DiscreteCosineTransformContext<float>(FULL_LENGTH);

    // Forward DCT
    ctx->forward(input_new, output_new);
    t.dct_4d(input_old, output_old, size, size);

    EXPECT_NEAR(distance<float>(output_new, output_old, FULL_LENGTH), 
        0, ERROR_EPSILON) << "The distance between the intermediate DCT "
                             "is bigger then the expected value.";

    std::swap(input_new, output_new);
    std::swap(input_old, output_old);
    
    // Inverse DCT
    ctx->inverse(input_new, output_new);
    t.idct_4d(input_old, output_old, size, size);

    EXPECT_NEAR(distance<float>(output_new, output_old, FULL_LENGTH), 
        0, ERROR_EPSILON) << "The distance between the resulting vectors is "
                             "bigger then the expected value.";
    
    delete ctx;
    DiscreteCosineTransformContext<float>::flush_coeff();
    #undef FULL_LENGTH 
}

TEST(ContextTest, new_dct_api_4d_is_consistent_to_old_api) {
    #define SIZE_X  15
    #define SIZE_Y  16
    #define SIZE_U  17
    #define SIZE_V  18
    #define FULL_LENGTH (SIZE_X * SIZE_Y * SIZE_U * SIZE_V)

    TransformContext<float> *ctx;

    Point4D size(SIZE_X, SIZE_Y, SIZE_U, SIZE_V);
    Point4D stride(1,
                   SIZE_X,
                   SIZE_X * SIZE_Y, 
                   SIZE_X * SIZE_Y * SIZE_U);
    float input_new[FULL_LENGTH];
    float input_old[FULL_LENGTH];
    float output_new[FULL_LENGTH];
    float output_old[FULL_LENGTH];

    // Seed the random number generator
    std::srand(0);

    // Populate input with random numbers and initialize output arrays.
    for (int i = 0; i < FULL_LENGTH; i++) {
        float value = 256 * (std::rand() / (RAND_MAX * 1.0));
        input_new[i] = value;
        input_old[i] = value;
        output_new[i] = 0;
        output_old[i] = 0;
    }
    
    old::Transform t(size);
    ctx = new DiscreteCosineTransformContext4D<float>(size, stride);

    // Forward DCT
    ctx->forward(input_new, output_new);
    t.dct_4d(input_old, output_old, size, size);

    EXPECT_NEAR(distance<float>(output_new, output_old, FULL_LENGTH), 
        0, ERROR_EPSILON) << "The distance between the intermediate DCT "
                             "is bigger then the expected value.";
    

    std::swap(input_new, output_new);
    std::swap(input_old, output_old);
    
    // Inverse DCT
    ctx->inverse(input_new, output_new);
    t.idct_4d(input_old, output_old, size, size);

    EXPECT_NEAR(distance<float>(output_new, output_old, FULL_LENGTH), 
        0, ERROR_EPSILON) << "The distance between the resulting vectors is "
                             "bigger then the expected value.";

    delete ctx;
    DiscreteCosineTransformContext<float>::flush_coeff();
    #undef SIZE_X
    #undef SIZE_Y
    #undef SIZE_U
    #undef SIZE_V
}

TEST(ContextTest, new_dct_API_works_for_blocks_of_different_sizes) {
    #define SIZE_X  15
    #define SIZE_Y  16
    #define SIZE_U  17
    #define SIZE_V  18
    #define FULL_LENGTH (SIZE_X * SIZE_Y * SIZE_U * SIZE_V)
    
    #define DELTA_SIZE 5

    TransformContext<float> *ctx;
    gtestlog->set_level(spdlog::level::debug);

    Point4D size_lightfield(SIZE_X, SIZE_Y, SIZE_U, SIZE_V);
    Point4D size_block = size_lightfield - DELTA_SIZE;
    Point4D stride_lightfield(1,
                             SIZE_X,
                             SIZE_X * SIZE_Y, 
                             SIZE_X * SIZE_Y * SIZE_U);
    float input_new[FULL_LENGTH];
    float input_old[FULL_LENGTH];
    float output_new[FULL_LENGTH];
    float output_old[FULL_LENGTH];

    // Seed the random number generator
    std::srand(0);

    // Populate input with random numbers and initialize output arrays.
    for (int i = 0; i < FULL_LENGTH; i++) {
        float value = 256 * (std::rand() / (RAND_MAX * 1.0));
        input_new[i] = value;
        input_old[i] = value;
        output_new[i] = 0;
        output_old[i] = 0;
    }
    
    auto *size_block_array = size_block.to_array();

    old::Transform t(size_lightfield);
    ctx = new DiscreteCosineTransformContext4D<float>(size_lightfield,
                                                      stride_lightfield);

    // Forward DCT
    ctx->forward(input_new, output_new, size_block_array);
    t.dct_4d(input_old, output_old, size_block, size_lightfield);

    EXPECT_NEAR(distance<float>(output_new, output_old, FULL_LENGTH), 
        0, ERROR_EPSILON) << "The distance between the intermediate DCT "
                             "is bigger then the expected value.";
    

    std::swap(input_new, output_new);
    std::swap(input_old, output_old);
    
    // Inverse DCT
    ctx->inverse(input_new, output_new, size_block_array);
    t.idct_4d(input_old, output_old, size_block, size_lightfield);

    EXPECT_NEAR(distance<float>(output_new, output_old, FULL_LENGTH), 
        0, ERROR_EPSILON) << "The distance between the resulting vectors is "
                             "bigger then the expected value.";
    
    delete[] size_block_array;
    delete ctx;
    DiscreteCosineTransformContext<float>::flush_coeff();
    #undef SIZE_X
    #undef SIZE_Y
    #undef SIZE_U
    #undef SIZE_V
    #undef FULL_LENGTH
    #undef DELTA_SIZE
}