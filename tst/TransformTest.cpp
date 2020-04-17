#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "TransformContext.h"
#include "DiscreteCosineTransformContext.h"
#include "DiscreteCosineTransformContext4D.h"
#include "utils.h"


#define ERROR_REC_EPSILON           1e-2

auto gtestlog = spdlog::basic_logger_mt("gtestlog", "logs/gtestlog.txt");

TEST(DCTContextTest, dct_error_is_within_1_percent)
{
    #define FULL_LENGTH 512 
    TransformContext<float> *ctx;

    float signal[FULL_LENGTH];
    float dct_signal[FULL_LENGTH];
    float recovered_signal[FULL_LENGTH];

    // Seed the random number generator
    std::srand(0);

    // Populate signal with random numbers.
    for (int i = 0; i < FULL_LENGTH; i++) {
        float value = 256 * (std::rand() / (RAND_MAX * 1.0));
        signal[i] = value;
        dct_signal[i] = 0;
        recovered_signal[i] = 0;
    }
    
    ctx = new DiscreteCosineTransformContext<float>(FULL_LENGTH);

    // Forward DCT
    ctx->forward(signal, dct_signal);
    ctx->inverse(dct_signal, recovered_signal);

    EXPECT_NEAR(distance<float>(signal, recovered_signal, FULL_LENGTH), 
        0, ERROR_REC_EPSILON) << "Distance between original and recovered is "
                                 "bigger than 1%.";
    
    delete ctx;
    DiscreteCosineTransformContext<float>::flush_coeff();
    #undef FULL_LENGTH 
}

TEST(DCTContextTest, dct_4d_error_is_within_1_percent)
{
    #define SIZE_X  15
    #define SIZE_Y  16
    #define SIZE_U  17
    #define SIZE_V  18
    #define FULL_LENGTH (SIZE_X * SIZE_Y * SIZE_U * SIZE_V)

    Point4D size(SIZE_X, SIZE_Y, SIZE_U, SIZE_V);
    Point4D stride(1,
                   SIZE_X,
                   SIZE_X * SIZE_Y, 
                   SIZE_X * SIZE_Y * SIZE_U);

    float signal[FULL_LENGTH];
    float dct_signal[FULL_LENGTH];
    float recovered_signal[FULL_LENGTH];

    TransformContext<float> *ctx;

    // Seed the random number generator
    std::srand(0);

    // Populate signal with random numbers.
    for (int i = 0; i < FULL_LENGTH; i++) {
        float value = 256 * (std::rand() / (RAND_MAX * 1.0));
        signal[i] = value;
        dct_signal[i] = 0;
        recovered_signal[i] = 0;
    }
    
    ctx = new DiscreteCosineTransformContext4D<float>(size, stride);

    // Forward DCT
    ctx->forward(signal, dct_signal);
    ctx->inverse(dct_signal, recovered_signal);

    EXPECT_NEAR(distance<float>(signal, recovered_signal, FULL_LENGTH), 
        0, ERROR_REC_EPSILON) << "Distance between original and recovered is "
                                 "bigger than 1%.";
    
    delete ctx;
    DiscreteCosineTransformContext<float>::flush_coeff();
    #undef FULL_LENGTH 
}